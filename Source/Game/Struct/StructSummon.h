#pragma once

#include <vector>

#include <toolkit/ILock.h>
#include <toolkit/XPropertySet.h>
#include <mmo/ArObject.h>

#include "ContentStruct.h"
#include "StructCreature.h"
#include "StructItem.h"
#include "GameRule.h"
#include "GameDBManager.h"

struct StructSummon : StructCreature, XPropertySet
{
	enum
	{
		SUMMON_SPAWN_LENGTH = 70,	// 플레이어 얼마나 근처에서 나타날지..
		SUMMON_SPAWN_MIN_LENGTH = 24,
		SUMMON_MAX_NON_ARTIFACT_ITEM_WEAR = 2,
		SUMMON_MAX_ACCESSORY_ITEM_WEAR = 3,
		SUMMON_MAX_ITEM_WEAR = 14,
	};

	static void				BindProperty();

	static StructSummon*	AllocSummon( struct StructPlayer* pMaster, unsigned idx = 0 );
	static void				FreeSummon( StructSummon* p );

	virtual bool			IsDeleteable();

	virtual bool			IsEnemy( StructCreature* pTarget, bool bIncludeHiding = false );
	virtual bool			IsVisible( StructCreature *pTarget );

	virtual bool			IsSkillCastable();
	virtual bool			IsMagicCastable();

	bool			IsRidable() const		{ return m_bIsRidable; }

	bool			SetSummonInfo( unsigned idx );	
	bool			IsSummon() const	{ return true; }

	int				GetSummonSID() const	{ return m_nSID; }	
	void			SetSummonSID( int sid ) { m_nSID = sid; }
	int				GetTransformLevel() const;
	int				GetEvolveTarget() const;
	int				GetSP() const	{ return m_nSP; }
	int				GetMaxSP() const{ return m_nMaxSP; }
	int				AddSP( int SP ) { return SetSP( GetSP() + SP ); }
	int				SetSP( int SP ) { int oldSP = m_nSP; m_nSP = SP; if( m_nSP > m_nMaxSP ) m_nSP = m_nMaxSP; if( m_nSP < 0 ) m_nSP = 0; if( oldSP != m_nSP ) onSPChange(); return m_nSP; }

	bool			IsAlly( StructCreature *pTarget );

	virtual void	AddExp( __int64 exp, __int64 jp, bool bApplyStamina = true, bool bForce = false );

	virtual int		GetSID() const		{ return GetSummonSID(); }
	virtual int		GetJobDepth() const;

	void			SetName( const char *szName );

	const SummonBase* GetSummonBase()	{ return m_pContentInfo; }
	char			GetRidingInfo() const;
	int				GetSummonCode();
	virtual const char*		GetName() const;
	virtual int		GetCreatureGroup() const;

	const char*		GetSummonName();
	int				GetSummonNameId();
	int				GetSummonType();
	int				GetSummonElementalType();

	virtual bool	IsBattleMode() const { return m_bIsBattleMode; }
	void			SetBattleModeOn()	{ m_bIsBattleMode = true; }
	void			SetBattleModeOff()	{ m_bIsBattleMode = false; }

	unsigned char	GetSummonSlotIndex()						{ return m_cSlotIndex;		}
	void			SetSummonSlotIndex( unsigned char c )		{ m_cSlotIndex = c;			}

	virtual const CreatureStat &	GetBaseStat() const;

	AR_HANDLE				GetHandle() const	{ return m_hHandle;	}

	virtual unsigned char	GetRace() const;

	virtual const c_fixed10	GetFCM() const								{ return m_fCM;			}
	void					SetMaster( struct StructPlayer* pMaster )	{ m_pMaster = pMaster;	}
	struct StructPlayer*	GetMaster() const							{ return m_pMaster;		}
	int						GetAccountId() const						{ return m_nAccountId;	}
	void					SetAccountId( int account_id )				{ m_nAccountId = account_id; }

	void					SetParentCard( struct StructItem *pItem )	{ m_pItem = pItem;		}
	struct StructItem*		GetParentCard() const						{ return m_pItem;		}

	int						GetItemSlotCount() const					{ return m_nMaxItemWear; }

	void					SetSummonFlag( bool bFlag )					{ m_bIsSummoned = bFlag; }
	bool					GetSummonFlag()	const						{ return m_bIsSummoned; }

	bool					DoEvolution( int nTargetCode = 0 );
	bool					Reborn();

	__int64					GetDeadEXPPenalty();
	bool					IsEXPLimitReached();

	unsigned char			GetRidingMoveSpeed();
	char					GetRate() const;
	int						GetUnMountProbabilityOnDamage()				{ return m_nUnMountProbabilityOnDamage; }
	int						GetUnMountProbabilityOnCriticalDamage()		{ return m_nUnMountProbabilityOnCriticalDamage; }

	virtual int		onDamage( StructCreature * pFrom, Elemental::Type elementalType, DamageType damageType, int nDamage, bool bCritical, bool bDouble);

	// DB 쿼리 관련
	void			DBQuery( struct GameDBManager::DBProc *pWork );
	void			onEndQuery();

	virtual bool			IsUsingBow() const;
	virtual bool			IsUsingCrossBow() const;

	virtual bool	TranslateWearPosition( ItemBase::ItemWearType & pos, struct StructItem* pItem, std::vector< int > * vpOverlappItemList = NULL );
	virtual struct StructItem*		GetWearedItem( ItemBase::ItemWearType idx ) const;
	virtual ItemBase::ItemWearType	GetAbsoluteWearPos( ItemBase::ItemWearType & pos );

	// 에테리얼 내구도 관련
	virtual void	ProcEtherealDurabilityConsumption( const bool bIsAttacker, const DamageType eDamageType, const int nDamage );

	virtual float	GetScale() const;
	virtual float	GetSize() const;

	// 크리처 강화 관련
	virtual int		GetEnhance() const				{ return GetParentCard()?GetParentCard()->GetItemEnhance():0; }

	virtual c_fixed10		CalcAttributeSum( const int bitset1, const int bitset2, const CreatureAttributeServer& attrBeforeCalc );

protected:

	// ArScheduler 에서 StructSummon 를 지우기 위해 존재함.
	// alloc/free 메커니즘에 관한 확실한 이해 전에는 수정 말것!
	virtual bool			ProcDelete();

	virtual void			onSPChange();
	virtual void			onExpChange();
	virtual void			onEnergyChange();
	virtual void			onCantAttack( AR_HANDLE target, AR_TIME t );
	virtual void			onDead( StructCreature *pFrom, bool decreaseEXPOnDead = true );
	virtual void			onJPChange();
	virtual void			onRegisterSkill( int skill_uid, int skill_id, int skill_level, bool is_new_skill );

	virtual unsigned short	putonItem( ItemBase::ItemWearType pos, struct StructItem * pItem );	// insert absolute position
	virtual unsigned short	putoffItem( ItemBase::ItemWearType pos );	// insert absolute position

	// 스킬/직업 초기화 관련
	virtual __int64			getJPAfterSkillReset();
	virtual int				IsLearnableSkill( int nSkillID, int nSkillLevel, int *nSkillTreeID );
	virtual void			onResetSkill( const int jobDepth );
	virtual void			onRemoveSkill( StructSkill* removedSkill );

	virtual void			procMoveSpeedChangement();

	virtual void			onAfterRemoveState( StructState & state, const bool bByDead = false );
	virtual void			onUpdateState( StructState & state, bool bIsExpire = false );

	virtual void			onChangeProperty( const std::string & strKey, const char *data );

	virtual bool IsAttackable()
	{
		if( GetRidingInfo() == SummonBase::RIDING_LENT ) return false;
		return StructCreature::IsAttackable();
	}

	virtual const char* getClassName()	{ return "StructSummon"; }

	// { Summon AI
	virtual void			onProcess( int nThreadIdx );
	void			processWalk( AR_TIME t );
	// }

	StructSummon( const char *szDummy )
	{
		m_nSP = m_nMaxSP = 0;
		m_nSPAdd = 0;
		m_nSPRegenAdd = 0;

		m_bIsRidable = 0;
		m_fRideSpeedMod = 0;
		m_nUnMountProbabilityOnDamage = 0;
		m_nUnMountProbabilityOnCriticalDamage = 0;

		m_fSoulConnectionRatio = 0;
		m_nAccountId = 0;
		m_pMaster = NULL;
		m_pItem = NULL;

		m_pContentInfo = NULL;

		m_nMaxItemWear = 0;
		m_bIsSummoned = false;
		memset( m_szName, 0, sizeof( m_szName ) );

		m_hHandle = 0;

		m_nSID = 0;

		m_nTransformLevel = 0;
		m_nStatus = 0;

		m_cSlotIndex = 0;

		m_nLastCantAttackTime = 0;
		m_nLastProcessTime = 0;

		m_bIsBattleMode = false;
		m_fMaxSPAmplifier = 0;
		m_fSPRegenAddAmplifier = 0;

#ifdef _MEM_USAGE_DEBUG
		XSEH::IncreaseAllocCount( "StructSummon" );
#endif
	};	// bind property 전용
	StructSummon( AR_HANDLE handle, unsigned idx );
	virtual	~StructSummon();
	
	int									m_nSP, m_nMaxSP;
	short								m_nSPAdd;
	short								m_nSPRegenAdd;

	c_fixed10							m_fBaseAttackPointRatio;
	c_fixed10							m_fBaseMagicPointRatio;
	c_fixed10							m_fBaseDefenceRatio;
	c_fixed10							m_fBaseMagicDefenceRatio;

	c_fixed10							m_fCM;

	bool								m_bIsRidable;
	float								m_fRideSpeedMod;
	int									m_nUnMountProbabilityOnDamage;
	int									m_nUnMountProbabilityOnCriticalDamage;

	float								m_fSoulConnectionRatio;
	int									m_nAccountId;
	struct StructPlayer*				m_pMaster;
	struct StructItem*					m_pItem;

	struct SummonBase*					m_pContentInfo;

	int									m_nMaxItemWear;
	bool								m_bIsSummoned;
	char								m_szName[61];

	AR_HANDLE							m_hHandle;

	int									m_nSID;

	int									m_nTransformLevel;
	int									m_nStatus;

	unsigned char						m_cSlotIndex;

	AR_TIME								m_nLastCantAttackTime;
	AR_TIME								m_nLastProcessTime;

	bool								m_bIsBattleMode;

	XCriticalSection							m_bQueryLock;
	std::list< struct GameDBManager::DBProc* >	m_lQueryList;

	friend struct DB_Login;
	friend struct DB_UpdateSummon;
	friend struct DB_ReadStorageList;

	// 스탯 관련 함수 및 변수들을 아래에 정리
protected:
	// --- 초기화 관련 루틴 ---
	virtual void			calcAttribute( CreatureAttributeServer* pAttribute );

	// --- 스탯 및 능력치 연산에 관련된 패시브 스킬들 적용하는 루틴 ---
	virtual void			applyPassiveSkillEffect();
	virtual void			applyPassiveSkillAmplifyEffect();
	virtual void			calcParamBasedStat();
	virtual void			calcParamBasedAttribute();

	// --- 스탯 및 능력치 연산에 관련된 지속 효과들 적용하는 루틴 ---
	virtual void			applyState( StructState & state );
	virtual void			applyStatByState();
	
	// --- 스탯 및 능력치 연산에 관련된 아이템 효과를 적용하는 루틴 ---
	virtual void			_applyEffectForState( const EffectInfo * pEffect );	
	virtual void			onItemWearEffect( struct StructItem* pItem, bool bIsBaseVar, int type, c_fixed10 var1, c_fixed10 var2, float fRatio );

	// --- 기타 예외적인 처리 ---
	virtual void			applyJobLevelBonus();

	// --- 이하 핸들러들 ---
	virtual void			onBeforeCalculateStat();
	virtual void			onAfterApplyStat();
	virtual void			onApplyStat();	
	virtual void			onModifyStatAndAttribute();
	virtual	void			onCompleteCalculateStat();

	// --- 각종 스탯을 계산하는 유틸리티 함수들 ---
	virtual void			incParameter( const int nBitset, const c_fixed10 & fValue, const bool bStat, ParameterForSummon *pParam = NULL );
	virtual void			ampParameter( const int nBitset, const c_fixed10 & fValue, const bool bStat );


	float								m_fMaxSPAmplifier;	
	float								m_fSPRegenAddAmplifier;	
	std::vector< struct StructSkill * >		m_vMasterAmplifyPassiveSkillList;
};