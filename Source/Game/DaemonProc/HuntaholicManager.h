#pragma once

#include <vector>
#include <set>

#include <toolkit/ILock.h>

#include "StructPlayer.h"
#include "StructMonster.h"
#include "StructFieldProp.h"
#include "GameContent.h"


struct HuntaholicManager : public ArSchedulerObject
{
public:
	enum _HUNTING_RESULT
	{
		HUNTING_RESULT_UNKNOWN					= -1,	// QuitHunting 함수 안에서 플레이어의 상태에 따라서 알아서 판단하도록 함

		HUNTING_RESULT_SUCCESS					= 0,
		HUNTING_RESULT_FAILED_BY_DEATH			= 1,
		HUNTING_RESULT_RETIRED					= 2,
		HUNTING_RESULT_NOREWARD					= 3,
		HUNTING_RESULT_FAILED_BY_GAMETIME_LIMIT	= 4
	};

protected:
	struct HuntaholicInfo;

	// 게임 내에서 사용하는 인스턴스화 된 헌터홀릭 던전 정보
	struct InstanceDungeon : public StructMonster::MonsterDeleteHandler, StructFieldProp::FieldPropDeleteHandler
	{
		InstanceDungeon( HuntaholicInfo * _pHuntaholicInfo, const GameContent::HUNTAHOLIC_INSTANCE_BASE * _pInstanceBase, const unsigned char _nInstanceNo, const int _nPartyID, const unsigned char _nMaxMemberCount, const char * _szPassword )
			: pHuntaholicInfo( _pHuntaholicInfo )
			, pInstanceBase( _pInstanceBase )
			, nInstanceNo( _nInstanceNo )
			, nPartyID( _nPartyID )
			, nMaxMemberCount( _nMaxMemberCount )
			, csInstance( "HuntaholicManager::InstanceDungeon::csInstance" )
			, bBegin( false )
			, tBeginTime( 0 )
			, bEndNoticed( false )
			, bEnd( false )
			, nKillCount( 0 )
			, nScore( 0 )
		{
			s_strcpy( szPassword, _countof( szPassword ), _szPassword );
		}

		virtual void			onMonsterDelete( StructMonster * pMonster );
		virtual void			onFieldPropDelete( StructFieldProp * pProp );
		void					procRespawn();
		void					broadcastInstanceDungeonInfoToMembers() const;

		const unsigned short	joinInstanceDungeon( StructPlayer * pPlayer, const char * pszPassword );
		const unsigned short	leaveInstanceDungeon( StructPlayer * pPlayer );

		const unsigned short	beginHunting();
		const unsigned short	quitHunting( StructPlayer * pPlayer, const bool bNeedToReward, const _HUNTING_RESULT eResult, int * pnSuppliedRewardPoint );
		const unsigned short	endHunting();

		void					procMonsterRespawn( const AR_TIME & tCurrent );
		void					procHealingPropRespawn( const AR_TIME & tCurrent );

	protected:
		void					clearMonsters();
		void					clearHealingProps();

	public:
		// 사냥 점수 관련
		struct _SCORE_TAG
		{
			_SCORE_TAG( const AR_HANDLE & _hOwner )	: hOwner( _hOwner ), nKillCount( 0 ), nScore( 0 )	{}

			AR_HANDLE		hOwner;
			int				nKillCount;
			int				nScore;
		};

		_SCORE_TAG *		getScoreTag( const AR_HANDLE & hPlayer );
		const _SCORE_TAG *	getScoreTag( const AR_HANDLE & hPlayer ) const;

		HuntaholicInfo *									pHuntaholicInfo;	// onMonsterDelete/onFieldPropDelete에서 HuntaholicInfo에 접근할 수 있도록 하기 위함
		const GameContent::HUNTAHOLIC_INSTANCE_BASE *		pInstanceBase;		// 인스턴스 던전 관련 정보
		const unsigned char									nInstanceNo;		// 방 만들 때 할당되지만 실제로 사냥 던전의 레이어로도 함께 사용(미니맵에 레이어 번호가 방번호와 일치하도록 함)
		const int											nPartyID;
		const unsigned char									nMaxMemberCount;
		char												szPassword[ GameRule::HUNTAHOLIC_MAX_INSTANCE_PASSWORD_LENGTH + 1 ];

		// 아래의 멤버를 보호(지역 락과의 순서: csInstance -> 지역 락 / 파티 락과의 순서: csInstance -> 파티 락)
		XCriticalSection								csInstance;

		// { 접근할 때 반드시 csInstance 걸어야 함
		bool											bBegin;			// 사냥 시작 여부(몬스터/힐링프랍 리젠, 던전 입장 처리 완료 여부와 동일)
		AR_TIME											tBeginTime;		// 사냥 시작 시각(시작 버튼 누른 시점에 10초 뒤로 설정됨)
		bool											bEndNoticed;	// 사냥 종료 예정 공지 여부
		bool											bEnd;			// 사냥 종료 여부(던전이 삭제되어야 할 시점에 세팅되며, HuntaholicManager::onProcess에서 던전을 삭제 대기 목록으로 옮기기 위한 플래그)

		int												nKillCount;		// 방 전체 킬 카운트
		int												nScore;			// 방 전체 획득 점수

		std::vector< _SCORE_TAG >						vScoreTag;

		typedef std::pair< const GameContent::HUNTAHOLIC_MONSTER_RESPAWN_INFO *, AR_TIME >	PendedMonsterRespawnInfo;
		std::vector< PendedMonsterRespawnInfo >			vPendedMonsterRespawn;
		std::vector< StructMonster * >					vRespawnedMonster;

		typedef std::pair< const GameContent::FIELD_PROP_RESPAWN_INFO *, AR_TIME >			PendedHealingPropRespawnInfo;
		std::vector< PendedHealingPropRespawnInfo >		vPendedHealingPropRespawn;
		std::vector< StructFieldProp * >				vRespawnedHealingProp;
		// } 접근할 때 반드시 csInstance 걸어야 함
	};

	// 게임 내에서 사용하는 헌터홀릭 정보
	struct HuntaholicInfo
	{
		HuntaholicInfo( const GameContent::HUNTAHOLIC_BASE * _pHuntaholicBase )
			: pHuntaholicBase( _pHuntaholicBase )
		{}

		const bool				IsInstanceFull() const			{ return stInstanceNo.size() >= GameRule::HUNTAHOLIC_MAX_INSTANCE_COUNT; }
		const size_t			GetFreeInstanceCount() const	{ return ( IsInstanceFull() ) ? 0 : GameRule::HUNTAHOLIC_MAX_INSTANCE_COUNT - stInstanceNo.size(); }
		const unsigned char		GetNewInstanceNo() const;

		void					ProcPendedMonsterRespawn();

		const GameContent::HUNTAHOLIC_BASE * pHuntaholicBase;

		// 몬스터 리젠을 예약/처리하기 위한 vPendedRespawn를 보호(지역 락 -> csPendedRespawn 순서여야 함)
		XCriticalSection									csPendedRespawn;
		std::vector< GameContent::PENDED_RESPAWN_INFO * >	vPendedRespawn;

		// 이하 변수들 접근할 때 반드시 걸어야 함(csInstanceDungeon -> InstancedDungeon::csInstance -> 지역 락 순서여야 함)
		XCriticalSection						csInstanceDungeon;
		std::vector< InstanceDungeon * >		vInstanceDungeon;					// 현재 생성되어 있는 방 정보 리스트
		std::vector< InstanceDungeon * >		vPendedInstanceDungeonToBeDeleted;	// 삭제 대기 중인 방 정보 리스트(종료 즉시 삭제하면 처리 대기 중이었던 쓰레드에서 문제가 생기므로 임시 보관)
		std::set< unsigned char >				stInstanceNo;						// 사용 중인 방 번호 목록(새 방 번호 발급 관련)
	};

public:
	~HuntaholicManager();

	static HuntaholicManager & Instance();

	const bool				Init();
	const bool				DeInit();
	const bool				IsInitialized() const;

	const bool				ClearHuntaholicInfo();
	const bool				RegisterHuntaholicBase( const GameContent::HUNTAHOLIC_BASE * pHuntaholicBase );

	const int				GetHuntaholicID( const ArPosition & pos ) const;
	const bool				IsHuntaholicLobby( const ArPosition & pos ) const;
	const bool				IsHuntaholicDungeon( const ArPosition & pos ) const;

	const ArPosition		GetLobbyPosition( const int nHuntaholicID ) const;
	const unsigned char		GetProperLobbyLayer( const int nHuntaholicID, const int nLevel ) const;

	const size_t			GetFreeInstanceCount( const int nHuntaholicID ) const;
	const size_t			GetUsedInstanceCount( const int nHuntaholicID, const int nLevel = 0 ) const;
	const size_t			GetInstanceList( const int nHuntaholicID, const int nLevel, const int nPage, TS_HUNTAHOLIC_INSTANCE_INFO * pBuffer );

	const unsigned short	CreateInstanceDungeon( const int nHuntaholicID, struct StructPlayer * pPlayer, const char * szName, const int nMaxMember, const char * szPassword );
	const unsigned short	JoinInstanceDungeon( const int nHuntaholicID, struct StructPlayer * pPlayer, const int nInstanceNo, const char * szPassword );
	const unsigned short	LeaveInstanceDungeon( const int nHuntaholicID, struct StructPlayer * pPlayer );

	const unsigned short	BeginHunting( const int nHuntaholicID, struct StructPlayer * pPlayer );
	const unsigned short	QuitHunting( const int nHuntaholicID, struct StructPlayer * pPlayer, const bool bNeedToReward = true, const _HUNTING_RESULT eResult = HUNTING_RESULT_UNKNOWN );

	// 데드락 문제를 피하기 위해 몬스터 리젠을 스킬, 스크립트 등에서 직접 처리하지 않고 큐에 쌓았다가 처리하도록 하기 위한 함수
	void					PendRespawnMonster( const int nHuntaholicID, const AR_UNIT x, const AR_UNIT y, const unsigned char nLayer, const int nMonsterID, const int nRespawnCount, const AR_TIME nLifeTime, const AR_HANDLE hInitialEnemy );

	virtual void			onProcess( int nThreadIdx );

protected:
	HuntaholicManager()		{}

	// 인스턴스 던전 정보 구조체(TS_HUNTAHOLIC_INSTANCE_INFO) 생성용
	static void		assembleInstanceInfo( const InstanceDungeon * pInstance, TS_HUNTAHOLIC_INSTANCE_INFO * pInfo );

private:
	// 락은 없으나 서버 실행 중에 변경되면 안 되는 놈이므로 변경 시도가 발생한 것 자체부터 미스 'ㅠ '
	std::vector< HuntaholicInfo * >		m_vHuntaholicInfo;

};
