#pragma once

#include <vector>

#include <mmo/ArObject.h>
#include <geometry/X2DPolygon.h>
#include <toolkit/XPropertySet.h>

#include "StructCreature.h"
#include "GameRule.h"
#include "GameContent.h"


struct _DAMAGE_TAG
{
	_DAMAGE_TAG( AR_TIME n, AR_HANDLE _uid, int d ) : nTime( n ), uid( _uid ), nDamage( d )
	{
	}

	AR_HANDLE		 uid;
	AR_TIME			 nTime;
	int				 nDamage;
};

struct _HATE_TAG
{
	_HATE_TAG( AR_TIME n, AR_HANDLE _uid, int h ) : nTime( n ), uid( _uid ), nHate( h ), bIsActive( true ), nBadAttackCount( 0 ), nLastMaxHate( 0 )
	{
	}

	AR_HANDLE		uid;
	AR_TIME			nTime;
	int				nHate;
	bool			bIsActive;
	int				nBadAttackCount;
	int				nLastMaxHate;
};

struct _HATE_MODIFIER_TAG
{
	_HATE_MODIFIER_TAG( AR_HANDLE _uid, int _hate ) : uid ( _uid ), nHate( _hate )
	{
	}

	AR_HANDLE		uid;
	int				nHate;
};

struct StructMonster : StructCreature, XPropertySet
{
	struct MonsterDeleteHandler
	{
		// 타이머(m_nLifeTime)에 의해 삭제되는 몬스터의 경우와 실제로 사냥 당해서 사망하는 경우 모두 호출 됨(몬스터 데이터 삭제 시)
		virtual void onMonsterDelete( struct StructMonster * pMonster ) = 0;
	};

	// 경험치 분배시 모든 공격자를 가상의 파티로 인식한다. 솔로 플레이어는 1인 파티가 된다.
	struct VIRTUAL_PARTY
	{
		VIRTUAL_PARTY( int id, int d, int lv ) : nPartyID( id ), nDamage( d ), nLevel( lv )
		{
			hPlayer = 0;
			fContribute = 0.0f;
			bTamer = false;
		}

		VIRTUAL_PARTY( AR_HANDLE h, int d, int lv  ) : hPlayer( h ), nDamage( d ), nLevel( lv )
		{
			nPartyID = 0;
			fContribute = 0.0f;
			bTamer = false;
		}

	//	bool operator <( const VIRTUAL_PARTY & rh ) const
	//	{
	//		return nDamage > rh.nDamage;
	//	}

		int			nPartyID;
		AR_HANDLE	hPlayer;
		int			nDamage;
		float		fContribute;
		bool		bTamer;
		int			nLevel;

		static bool greaterByDamage( const VIRTUAL_PARTY & lh, const VIRTUAL_PARTY & rh )
		{
			return lh.nDamage > rh.nDamage;
		}

		static bool greaterByContribute( const VIRTUAL_PARTY & lh, const VIRTUAL_PARTY & rh )
		{
			return lh.fContribute > rh.fContribute;
		}
	};

	enum MONSTER_GENERATE_CODE
	{
		BY_NONE	= 0,
		BY_RESPAWN,
		BY_SCRIPT,
		BY_SHOVELING,
	};

	enum MONSTER_STATUS
	{
		STATUS_NORMAL	= 0,
		STATUS_TRACKING,
		STATUS_FIND_ATTACK_POS,
		STATUS_ATTACK,		
		STATUS_DEAD,
		STATUS_RUNAWAY,
	};

	enum
	{
		ITEM_DROP_LENGTH = 18,	// 몹 얼마나 멀리에 아이템 드랍할건지..
	};

	virtual const char* getClassName()	{ return "StructMonster"; }
	static void	BindProperty();

	static StructMonster*	AllocMonster( unsigned idx, unsigned char difficulty = 0 );
	static void				FreeMonster( StructMonster* p );
	AR_HANDLE				GetHandle() const	{ return m_hHandle;	}

	virtual const char*		GetName() const;
	virtual int		GetCreatureGroup() const;

	void			SetGenerateCode( int code )						{ m_nGenerateCode = code;	}
	void			SetDeleteHandler( MonsterDeleteHandler * pHandler ) { m_pDeleteHandler = pHandler; }
	const MonsterDeleteHandler * GetDeleteHandler() const			{ return m_pDeleteHandler; }
	// 지정 시간 이후에 몬스터가 없어지도록 함, -1을 인자로 넘기면 지정되어 있던 타이머가 초기화 됨
	void			SetLifeTime( const AR_TIME nLifeTime )			{ m_nLifeTime = ( nLifeTime != INFINITE_TIME ) ? GetArTime() + nLifeTime : 0; }
	const bool		IsLifeTimeOver() const							{ return m_nLifeTime && m_nLifeTime <= GetArTime(); }

	void			SetReturnPosition( AR_UNIT x, AR_UNIT y )		{ m_RespawnX = x; m_RespawnY = y; }
	const ArPosition	GetReturnPosition()							{ return ArPosition( m_RespawnX, m_RespawnY ); }
	
	bool			SetMonsterInfo( unsigned idx );
	bool			IsMonster()	 const								{ return true; }
	bool			IsEnvironmentMonster() const;
	virtual bool	IsBattleMode() const { return ( GetStatus() == STATUS_TRACKING || GetStatus() == STATUS_ATTACK || GetStatus() == STATUS_FIND_ATTACK_POS ); }

	bool			IsBossMonster() const;
	int				GetMonsterType() const			{ return m_pContentInfo->monster_type; }
	bool			IsDungeonConnector() const;
	bool			IsAgent() const;
	bool			IsAutoTrap() const;
	bool			IsRunaway() const;
	bool			IsDungeonCore() const;
	AR_UNIT			GetChaseRange() const;

	int				GetDungeonId() const			{ return m_nDungeonId; }
	bool			IsDungeonRaidMonster() const	{ return m_bIsDungeonRaidMonster; }
	bool			IsOriginalDungeonSiegerGuardian() const	{ return m_bIsDungeonSiegerGuardian; }
	bool			IsOriginalDungeonOwnerGuardian() const	{ return m_bIsDungeonOwnerGuardian; }
	void			SetDungeonOwnerGuardian()		{ m_bIsDungeonOwnerGuardian = true; }
	void			SetDungeonSiegerGuardian()		{ m_bIsDungeonSiegerGuardian = true; }
	void			ResetDungeonOwnerGuardian()		{ m_bIsDungeonOwnerGuardian = false; }
	void			ResetDungeonSiegerGuardian()		{ m_bIsDungeonSiegerGuardian = false; }
	void			SetDungeonRaidMonster()			{ m_bIsDungeonRaidMonster = true; }
	void			SetDungeonId( int dungeon_id )	{ m_nDungeonId = dungeon_id; }

	// 헌터홀릭 던전 내 몬스터 처리 관련
	int				GetInstanceRespawnID() const							{ return m_nInstanceRespawnID; }
	void			SetInstanceRespawnID( const int nInstanceRespawnID )	{ m_nInstanceRespawnID = nInstanceRespawnID; }

	AR_UNIT			GetFirstAttackRange() const;

	bool			IsEnemy( StructCreature* pTarget, bool bIncludeHiding = false );
	bool			IsAlly( StructCreature* pTarget );
	bool			IsFirstAttacker() const;
	bool			IsGroupRevenger() const;
	bool			IsGroupFirstAttacker() const;
	bool			IsCastRevenger() const;
	bool			IsBattleRevenger() const;
	int				GetMonsterGroup() const;
	bool			IsTameable() const;
	ItemBase::ItemCode	GetTameItemCode() const;
	int				GetTameCode() const;
	const c_fixed10 GetTamePercetage() const;
	float			GetTameExpAdjust() const;

	bool			IsMonsterCreatureTame() const;
	int				GetMonsterCreatureTameCode() const;
	int				GetMonsterCreatureTameGroupCode() const;
	int				GetMonsterCreatureTameItemCode() const;

	virtual float	GetScale() const;
	virtual float	GetSize() const;
	
	MONSTER_STATUS	GetStatus() const;
	void			SetStatus( MONSTER_STATUS status );

	void			SetNeedToFindEnemy( bool bNeedToFindEnemy = true )		{ m_bNeedToFindEnemy = bNeedToFindEnemy; }

	virtual bool	StartAttack( AR_HANDLE target, bool bNeedFastReaction );

	virtual int		onDamage( StructCreature * pFrom, Elemental::Type elementalType, DamageType damageType, int nDamage, bool bCritical, bool bDouble);

	int				GetMonsterId();

	virtual const CreatureStat & GetBaseStat() const;
	virtual int				GetLevel() const;
	virtual unsigned char	GetRace() const;
	
	int				GetHate( AR_HANDLE handle );
	int				AddHate( AR_HANDLE handle, int pt, bool bBroadcast = true, bool bProcRoamingMonster = true );
	int				RemoveHate( AR_HANDLE handle, int pt );
	AR_HANDLE		GetEnemyUID();

	AR_HANDLE		GetMaxDamageDealer() const;

	virtual void	OnUpdate();

	AR_HANDLE		GetTamer() const;
	void			SetTamer( AR_HANDLE handle, StructSkill* pTamingSkill );
	StructSkill*	GetTamingSkill() const		{ return m_pTamingSkill; }
	int				GetTamingSkillLevel() const { return m_nTamingSkillLevel; }
	bool			IsTamingSuccessed() const	{ return m_bTamedSuccess; }

	void			SetWandering( bool IsWandering ) { m_bIsWandering = IsWandering; }

	virtual bool	IsAttackable();
	virtual bool	IsMovable();
	virtual bool	IsKnockbackable();
	virtual bool	IsSkillCastable();
	virtual bool	IsMagicCastable();

	// AI_processAttack에서 스크립트 호출시 공격 처리를 계속할지 멈출지 결정
	void			SetContinueAttack( bool bContinue )		{ m_bContinueAttack = bContinue; }
	bool			IsNeedToContinueAttack()				{ return m_bContinueAttack; }

	const MonsterBase *		GetMonsterBase()	{ return m_pContentInfo; }

	void			SetRespawnByScript() { m_bRespawnByScript = true; }

#ifndef _USE_NEW_ROAMING_ONLY
	void			SetWayPointInfo( const struct GameContent::WAY_POINT_INFO* pWayPointInfo )
	{
		m_pWayPointInfo = pWayPointInfo;
	}
#endif
	void					SetRoamer( struct StructRoamer * pRoamer )	{ m_pRoamer = pRoamer; }
	struct StructRoamer *	GetRoamer()									{ return m_pRoamer; }

	void			SetSkillTargetPosition( const ArPosition & pos )	{ m_posSkillTarget = pos; }
	ArPosition		GetSkillTargetPosition() const						{ return m_posSkillTarget; }
	void					comeBackHome(bool bInvincible);	// 응? -_-

protected:

	StructMonster( AR_HANDLE handle, unsigned idx, unsigned char difficulty = 0 );
	virtual	~StructMonster();

	// ArScheduler 에서 StructMonster 를 지우기 위해 존재함.
	// alloc/free 메커니즘에 관한 확실한 이해 전에는 수정 말것!
	virtual bool			ProcDelete();	

	// { Monster AI
	virtual void			onProcess( int nThreadIdx );
	virtual void			regenHPMP( AR_TIME t );

	bool					CheckTriggerCondition( const MonsterBase::MONSTER_TRIGGER & trigger, const int nTriggerIndex );
	bool					ResetTriggerCondition();

	void					AI_processAttack( AR_TIME t );
	void					AI_processAttack( StructCreature * pEnemy, AR_TIME t );
	void					processWalk( AR_TIME t );
	void					processMove( AR_TIME t );
	void					processDead( AR_TIME t );
	void					processFirstAttack( AR_TIME t );
	bool					findAttackablePosition( const ArPosition & myPosition, ArPosition & enemyPosition, AR_UNIT distance, AR_UNIT gap );
	void					findNextEnemy();
	bool					getMovePosition( ArPosition & pos );
	ArPosition				getNonDuplicateAttackPos( StructCreature * pEnemy );

	_HATE_TAG *				getHateTag( AR_HANDLE handle, AR_TIME t = 0 );
	_HATE_TAG *				addHate( AR_HANDLE handle, int nHate );
	bool					removeFromHateList( AR_HANDLE handle );
	void					clearHateList();
	void					updateHate();

	_DAMAGE_TAG *			getDamageTag( AR_HANDLE handle, AR_TIME t = 0 );
	_DAMAGE_TAG *			addDamage( AR_HANDLE handle, int nDamage );
    bool					removeFromDamageList( AR_HANDLE handle );

	void					procDropChaos( StructCreature * pKiller, std::vector< VIRTUAL_PARTY > & vPartyContribute, float fDropRatePenalty, float fPCBangDropRateBonus );
	void					procDropGold( const ArPosition & pos, StructCreature * pKiller, struct takePriority* pPriority, std::vector< VIRTUAL_PARTY > & vPartyContribute, float fDropRatePenalty, float fPCBangDropRateBonus );
	void					procDropItem( const ArPosition & pos, StructCreature * pKiller, struct takePriority* pPriority, std::vector< VIRTUAL_PARTY > & vPartyContribute, float fDropRatePenalty, float fPCBangDropRateBonus );

	const bool				selectDropItems( std::vector< int > & vItemID, std::vector< __int64 > & vItemCount );
	void					dropItemGroup( const ArPosition & pos, StructCreature * pKiller, struct takePriority* pPriority, std::vector< VIRTUAL_PARTY > & vPartyContribute, int nDropGroupID, const __int64 & count, int level, int nFlagIndex );
	void					dropItem( const ArPosition & pos, StructCreature * pKiller, struct takePriority* pPriority, std::vector< VIRTUAL_PARTY > & vPartyContribute, ItemBase::ItemCode code, const __int64 & count, int level, bool bIsEventItem, int nFlagIndex );

	void					calcPartyContribute( StructCreature * pKiller, std::vector< VIRTUAL_PARTY > & vPartyContribute );
	void					procImmoralPoint( std::vector< VIRTUAL_PARTY > & vPartyContribute );

	//struct StructCreature*				m_pEnemy;

	AR_HANDLE							m_hTamer;
	AR_TIME								m_nTamedTime;
	bool								m_bTamedSuccess;	// 테이밍이 성공적으로 완료 되었는지 여부
	StructSkill*						m_pTamingSkill;		// 테이밍 시도할 때 건 스킬
	int									m_nTamingSkillLevel;// 테이밍 시도할 때 사용된 스킬 레벨

	AR_TIME								m_nLastTrackTime;
	AR_TIME								m_nHateCheckTime;
	AR_HANDLE							m_hFirstAttacker;
	AR_TIME								m_nFirstAttackTime;
	std::vector< struct _DAMAGE_TAG >	m_vDamageList;
	std::vector< struct _HATE_TAG >		m_vHateList;
	std::vector< struct _HATE_MODIFIER_TAG >		m_vHateModifierByState;
	bool								m_bNeedToFindEnemy;
	int									m_nMaxHate;
	AR_TIME								m_nLastHateUpdateTime;
	// }
	bool								m_bComeBackHome;
	AR_TIME								m_nRegenTime;
	bool								m_bRespawnByScript;

	// { AI용
	AR_UNIT								m_nLastEnemyDistance;

	virtual void			onDead( StructCreature *pKiller, bool decreaseEXPOnDead = true );
	virtual void			procQuestAndTitle( const ArPosition & pos, StructCreature *pKiller, struct takePriority* pPriority, std::vector< VIRTUAL_PARTY > & vPartyContribute, float fDropRatePenalty );

	void					procEXP( StructCreature *pKiller, std::vector< VIRTUAL_PARTY > & vPartyContribute );
	

	struct MonsterBase*					m_pContentInfo;

	AR_HANDLE							m_hHandle;

	int									m_nTotalDamage;
	int									m_nStatus;
	int									m_nGenerateCode;
	bool								m_bIsWandering;

	AR_UNIT								m_RespawnX, m_RespawnY;

	bool								m_bNeedToSetReturnPos;
	MonsterDeleteHandler *				m_pDeleteHandler;
	AR_TIME								m_nLifeTime;

	int									m_nDungeonId;
	bool								m_bIsDungeonRaidMonster;
	bool								m_bIsDungeonOwnerGuardian;
	bool								m_bIsDungeonSiegerGuardian;

	int									m_nInstanceRespawnID;

	std::vector< int >					m_vTriggerFlag;

#ifndef _USE_NEW_ROAMING_ONLY
	const struct GameContent::WAY_POINT_INFO*	m_pWayPointInfo;

	int									m_nWayPointIdx;
#endif
	struct StructRoamer *				m_pRoamer;

	bool								m_bContinueAttack;

	unsigned char						m_nDifficulty;
	ArPosition							m_posSkillTarget;

	// 스탯 관련 함수 및 변수들을 아래에 정리
protected:
	// --- 스탯 및 능력치 연산에 관련된 지속 효과들 적용하는 루틴 ---
	virtual void			applyState( StructState & state );

	// --- 이하 핸들러들 ---
	virtual void			onBeforeCalculateStat();
	virtual void			onApplyAttributeAdjustment();
	virtual	void			onCompleteCalculateStat();
};