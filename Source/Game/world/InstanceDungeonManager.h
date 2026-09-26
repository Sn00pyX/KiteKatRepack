#pragma once

#include <vector>
#include <set>
#include <map>

#include <toolkit/ILock.h>
#include <mmo/ArSchedulerObject.h>
#include <mmo/ArcadiaServer.h>

#include "StructPlayer.h"
#include "StructMonster.h"
#include "StructFieldProp.h"
#include "GameContent.h"


struct InstanceDungeonManager : public ArSchedulerObject
{
	enum
	{
		MAX_INSTANCE			= 250,
		MAX_OBJECTIVE_INDEX		= 9,
	};

protected:
	struct InstanceDungeonInfo;

	struct InstanceDungeonType : public StructMonster::MonsterDeleteHandler, StructFieldProp::FieldPropDeleteHandler
	{
		InstanceDungeonType( InstanceDungeonInfo * _pInstanceDungeonInfo, const GameContent::INSTANCE_DUNGEON_TYPE_BASE * _pInstanceDungeonTypeBase, const unsigned char _nInstanceNo, const int _nKey, const int _nOwnerAccountID, const PlayerUID _nOwnerUID, const char *_szOwnerAccountName, const char *_szOwnerName, const unsigned char _nDifficulty )
			: pInstanceDungeonInfo( _pInstanceDungeonInfo )
			, pInstanceDungeonTypeBase( _pInstanceDungeonTypeBase )
			, nInstanceNo( _nInstanceNo )
			, nKey( _nKey )
			, nPlayerCount( 0 )
			, nDifficulty( _nDifficulty )
			, nOwnerAccountID( _nOwnerAccountID )
			, nOwnerUID( _nOwnerUID )
			, tDestructionTime( 0 )
		{
			s_strcpy( szOwnerAccountName, _countof( szOwnerAccountName ), _szOwnerAccountName );
			s_strcpy( szOwnerName, _countof( szOwnerName ), _szOwnerName );
		}

		virtual void									onMonsterDelete( StructMonster * pMonster );
		virtual void									onFieldPropDelete( StructFieldProp * pProp );

		const unsigned short							joinInstance( struct StructPlayer * pPlayer );
		const unsigned short							leaveInstance( struct StructPlayer * pPlayer );

		const unsigned short							beginInstance( struct StructPlayer * pPlayer );
		const unsigned short							endInstance();

		const int										doEachPlayer( ArObjectFunctor & functor );

		void											procMonsterRespawn( const AR_TIME & tCurrent );
		void											procHealingPropRespawn( const AR_TIME & tCurrent );

		// 인스턴스 던전 플래그 설정
		// empty value 값 = 0
		void											setFlag( const std::string & strName, const std::string & strValue );
		const std::string								getFlag( const std::string & strName ) const;

		int												getAliveRespawnGroupMonsterCount( const int nRespawnGroup ) const;

	protected:
		void											clearMonsters();
		void											clearHealingProps();
		void											clearItems();

	public:
		InstanceDungeonInfo *							pInstanceDungeonInfo;
		const GameContent::INSTANCE_DUNGEON_TYPE_BASE *	pInstanceDungeonTypeBase;
		const unsigned char								nInstanceNo;
		const unsigned char								nDifficulty;

		// 로그를 위한 인스턴스 던전의 소유주 정보
		const int										nOwnerAccountID;
		const PlayerUID									nOwnerUID;	
		char											szOwnerAccountName[GameRule::MAX_ACCOUNT_LEN + 1];
		char											szOwnerName[31];

		// 아래의 멤버를 보호(지역 락과의 순서: csInstanceDungeonType -> 지역 락 / 파티 락과의 순서: csInstanceDungeonType -> 파티 락)
		mutable XCriticalSection						csInstanceDungeonType;

		// { 접근할 때 반드시 csInstanceDungeonType 걸어야 함
		int																					nKey;
		int																					nPlayerCount;
		std::vector< StructPlayer * >														vPlayerList;

		AR_TIME																				tDestructionTime;	// 인스턴스 던전에 모든 파티원이 나간 시각

		typedef std::pair< const GameContent::INSTANCE_DUNGEON_MONSTER_RESPAWN_INFO *, AR_TIME >	PendedMonsterRespawnInfo;
		std::vector< PendedMonsterRespawnInfo >														vPendedMonsterRespawn;

		std::multimap< int , StructMonster * >												mapRespawnedMonster;

		// onMonsterDelete가 여러 쓰레드에서 동시 호출이 가능한데, 이 상태에서 몬스터 사망 스크립트를 호출하면
		// 사망 처리가 된 몬스터 수와 되지 않은 몬스터 수가 불분명해지기 때문에 onMonsterDelete 함수 내부에서
		// 스크립트 처리가 완료될 때까지 몬스터 제거 처리를 막는 락을 추가로 두고 사용함.
		// 이 락은 어떠한 락보다도 먼저 걸려 있어야 함.
		XCriticalSection								csMonsterDelete;

		typedef struct _PendedHealingPropRespawnInfo
		{
			_PendedHealingPropRespawnInfo( const GameContent::FIELD_PROP_RESPAWN_INFO *_pInfo, AR_TIME _tTime, bool _bStatic ) : pInfo( _pInfo ), tTime( _tTime ), bStatic( _bStatic )
			{}

			const GameContent::FIELD_PROP_RESPAWN_INFO *pInfo;
			AR_TIME tTime;
			bool bStatic;
		} PendedHealingPropRespawnInfo;

		std::vector< PendedHealingPropRespawnInfo >											vPendedHealingPropRespawn;
		std::vector< StructFieldProp * >													vRespawnedStaticProp;
		std::vector< StructFieldProp * >													vRespawnedProp;
		// } 접근할 때 반드시 csInstanceDungeonType 걸어야 함

	private:
		std::map< std::string , std::string >					m_mapFlags;
	};

	struct InstanceDungeonInfo
	{
		InstanceDungeonInfo( const GameContent::INSTANCE_DUNGEON_BASE * _pInstanceDungeonBase )
			: pInstanceDungeonBase( _pInstanceDungeonBase )
		{}

		const bool			IsInstanceFull() const			{ return mapInstanceNo.size() >= MAX_INSTANCE; }
		const unsigned char	GetNewInstanceNo() const;

		void				ProcPendedMonsterRespawn();
		void				ProcPendedPropRespawn();

		ArcadiaLock			LockWholeDungeonArea( short nLayer = -1 ) const;

		const GameContent::INSTANCE_DUNGEON_BASE *				pInstanceDungeonBase;

		// 몬스터 리젠을 예약/처리하기 위한 vPendedRespawn를 보호(지역 락 -> csPendedRespawn 순서여야 함)
		XCriticalSection										csPendedRespawn;
		std::vector< GameContent::PENDED_RESPAWN_INFO * >		vPendedRespawn;

		// 프랍 리젠을 예약/처리하기 위한 vPendedRespawnProp를 보호(지역 락 -> csPendedRespawnProp 순서여야 함)
		XCriticalSection										csPendedRespawnProp;
		std::vector< GameContent::FIELD_PROP_RESPAWN_INFO * >	vPendedRespawnProp;

		// 이하 변수들 접근할 때 반드시 걸어야 함(csInstanceDungeon -> InstancedDungeon::csInstance -> 지역 락 순서여야 함)
		mutable XCriticalSection				csInstanceDungeonType;
		std::map< int, InstanceDungeonType * >	mapInstanceDungeonType;					// 현재 생성되어 있는 던전 인스턴스
		std::vector< InstanceDungeonType * >	vPendedInstanceDungeonTypeToBeDeleted;	// 삭제 대기 중인 던전 인스턴스 리스트(종료 즉시 삭제하면 처리 대기 중이었던 쓰레드에서 문제가 생기므로 임시 보관)
		std::map< unsigned char, InstanceDungeonType * >
												mapInstanceNo;							// 사용 중인 던전 인스턴스의 번호 목록
	};

	const InstanceDungeonInfo *				findInstanceDungeonInfo( const ArPosition & pos ) const;
	const InstanceDungeonInfo *				findInstanceDungeonInfo( int nInstanceDungeonID ) const;
	InstanceDungeonInfo *					findInstanceDungeonInfo( int nInstanceDungeonID );
	const InstanceDungeonType *				findInstanceDungeonType( const InstanceDungeonInfo * pInstanceDungeonInfo, const unsigned char nLayer ) const;
	InstanceDungeonType *					findInstanceDungeonType( InstanceDungeonInfo * pInstanceDungeonInfo, const unsigned char nLayer );

public:
	~InstanceDungeonManager();

	static InstanceDungeonManager &			Instance();

	bool									Init();
	bool									DeInit();
	const bool								IsInitialized() const;

	const bool								ClearInstanceDungeonInfo();
	const bool								RegisterInstanceDungeonBase( const GameContent::INSTANCE_DUNGEON_BASE * pInstanceDungeonBase );

	const int								GetInstanceDungeonID( const ArPosition & pos ) const;
	const int								GetInstanceDungeonTypeID( int nInstanceDungeonID, const unsigned char nLayer ) const;

	const bool								GetInstanceDungeonPosition( const int nInstanceDungeonID, ArPosition & pos ) const;

	//인스턴스 던전 플래그
	//empty value 값 = 0
	const bool								SetInstanceDungeonTypeFlag( int nInstanceDungeonID, const unsigned char nLayer, const std::string & strName, const std::string & strValue );
	std::string								GetInstanceDungeonTypeFlag( int nInstanceDungeonID, const unsigned char nLayer, const std::string & strName ) const;

	// 여기서 Difficulty란 인스턴스 던전의 nID를 말함. 네이밍을 좀 더 직관적으로 수정해야 될 듯
	const int								GetInstanceDungeonDifficulty( const int nInstanceDungeonID, const unsigned char nLayer ) const;

	const int								GetAliveInstanceDungeonRespawnGroupMonsterCount( int nInstanceDungeonID, const unsigned char nLayer, const int nRespawnGroup ) const;

	const unsigned short					CreateInstanceDungeon( int nInstanceDungeonID, struct StructPlayer * pPlayer, int nType, const unsigned char nDifficulty );
	const unsigned short					JoinInstanceDungeon( int nInstanceDungeonID, struct StructPlayer * pPlayer, ArPosition & posEnter, unsigned char & nEnterLayer, bool bStorePosition = true );
	const unsigned short					LeaveInstanceDungeon( int nInstanceDungeonID, struct StructPlayer * pPlayer );

	const unsigned short					QuitInstanceDungeon( int nInstanceDungeonID, struct StructPlayer * pPlayer );

	const int								DoEachPlayerInInstanceDungeon( int nInstanceDungeonID, const unsigned char nLayer, ArObjectFunctor & functor );

	// 데드락 문제를 피하기 위해 몬스터 리젠을 스킬, 스크립트 등에서 직접 처리하지 않고 큐에 쌓았다가 처리하도록 하기 위한 함수
	void									PendRespawnMonster( const int nInstanceDungeonID, const AR_UNIT x, const AR_UNIT y, const unsigned char nLayer, const int nMonsterID, const int nRespawnCount, const AR_TIME nLifeTime, const AR_HANDLE hInitialEnemy );
	bool									PendRespawnMonster( const int nRespawnID, const int nInstanceDungeonID, const int nLayer );
	void									PendRespawnProp( const int nInstanceDungeonID, const int _nPropId, const AR_UNIT _x, const AR_UNIT _y, const unsigned char _layer, const float _fZOffset, const float _fRotateX, const float _fRotateY, const float _fRotateZ, const float _fScaleX, const float _fScaleY, const float _fScaleZ, const bool _bLockHeight, const float _fLockHeight );

	virtual void							onProcess( int nThreadIdx );

protected:
	// 락은 없으나 서버 실행 중에 변경되면 안 되는 놈이므로 변경 시도가 발생한 것 자체부터 미스 'ㅠ '
	std::vector< InstanceDungeonInfo * >	m_vInstanceDungeonInfo;

	// 로그 작성을 위해 유저 관련 정보를 보관하기 위한 구조체
	struct PlayerBasicInfoForLog
	{
		PlayerBasicInfoForLog( StructPlayer * _pPlayer )
			: pPlayer( _pPlayer )
			, gold( _pPlayer->GetGold() )
			, exp( _pPlayer->GetEXP() )
			, jp( _pPlayer->GetJobPoint() )
		{}

		bool operator != ( const StructPlayer * _pPlayer ) const
		{
			return ( pPlayer != _pPlayer );
		}
		bool operator != ( const PlayerBasicInfoForLog & rhs ) const
		{
			return ( pPlayer != rhs.pPlayer || gold != rhs.gold || exp != rhs.exp || jp != rhs.jp );
		}

		StructPlayer *		pPlayer;
		StructGold			gold;
		__int64				exp;
		__int64				jp;
	};
};
