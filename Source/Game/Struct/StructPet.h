#pragma once

#include <vector>

#include <toolkit/ILock.h>
#include <toolkit/XPropertySet.h>

#include "StructCreature.h"
#include "GameDBManager.h"


struct StructPet : public StructCreature, XPropertySet
{
	enum
	{
		PET_SPAWN_LENGTH = 70,	// 플레이어 얼마나 근처에서 나타날지..
		PET_SPAWN_MIN_LENGTH = 24,
	};

	enum SHOVELING_STATUS
	{
		SHOVELING_STATUS_IDLE			= 0,		// 삽질 안 하고 있는 상태
		SHOVELING_STATUS_SEARCH			= 1,		// 주변 탐색 단계
		SHOVELING_STATUS_APPROACH		= 2,		// 이동 단계
		SHOVELING_STATUS_DIG			= 3			// 땅 파기 단계
	};

	static void				BindProperty();

	static StructPet*		AllocPet( struct StructPlayer* pMaster, unsigned int code );
	static void				FreePet( StructPet* p );

	AR_HANDLE				GetHandle() const	{ return m_hHandle;	}

	virtual bool			IsDeleteable();

	virtual bool			IsEnemy()								{ return false; }
	virtual bool			IsAlly( StructCreature *pTarget )		{ return false; }


	bool					SetPetInfo( unsigned int code );	
	const PetBase &			GetPetBase() const			{ return *m_pContentInfo; }
	int						GetPetCode()				{ return m_pContentInfo->uid; }
	int						GetNameID() const			{ return m_pContentInfo->name_id; }
	virtual int				GetCreatureGroup() const	{ return (m_nChangingGroup == CREATURE_NONE) ? m_pContentInfo->type : m_nChangingGroup; }
	virtual unsigned char	GetRace() const				{ return CREATURE_ETC; }
	virtual bool			IsPet() const				{ return true; }

	void					SetPetSID( int sid )	{ m_nSID = sid; }
	int						GetPetSID() const		{ return m_nSID; }	
	virtual int				GetSID() const			{ return GetPetSID(); }

	void					SetName( const char *szName );
	void					SetName( const int nNameId );
	const char*				GetPetName() const				{ return m_szName; }
	virtual const char*		GetName() const					{ return GetPetName(); }
	void					SetNameChanged( const bool bNameChanged )	{ m_bNameChanged = bNameChanged; }
	const bool				IsNameChanged()								{ return m_bNameChanged; }
	const unsigned short	ChangeName( const char *szName, const bool bForce );

	void					SetMaster( struct StructPlayer* pMaster )	{ m_pMaster = pMaster;	}
	struct StructPlayer*	GetMaster() const							{ return m_pMaster;		}

	void					SetParentCage( struct StructItem *pItem )	{ m_pItem = pItem;		}
	struct StructItem*		GetParentCage() const						{ return m_pItem;		}

	void					SetSummonFlag( bool bFlag )					{ m_bIsSummoned = bFlag; }
	bool					GetSummonFlag()	const						{ return m_bIsSummoned; }

	virtual float			GetScale() const							{ return m_pContentInfo->scale; }
	virtual float			GetSize() const								{ return m_pContentInfo->size; }

	const bool				IsRare() const								{ return m_pContentInfo->attribute_flag & PetBase::RATE_RARE; }
	const bool				IsShovelable() const						{ return m_pContentInfo->attribute_flag & PetBase::RATE_SHOVELABLE; }
	const bool				IsItemCollectable() const					{ return m_pContentInfo->attribute_flag & PetBase::RATE_ITEM_COLLECTABLE; }
	// 소지량과 관련한 시작 인덱스부터 순차적으로 1000, 2000, 4000, 8000 ... 를 의미한다.
	const int				GetAddedWeight() const;

	const SHOVELING_STATUS	GetShovelingStatus()									{ return m_nShovelingStatus; }
	void					SetShovelingStatus( const SHOVELING_STATUS status )		{ m_nShovelingStatus = status; }

	virtual void			OnUpdate()									{}

	virtual bool			IsAttackable()								{ return false; }
	virtual bool			IsMovable()									{ return !IsUsingSkill(); }
	virtual bool			IsKnockbackable()							{ return false; }
	virtual bool			IsItemUseable()								{ return false; }
	virtual bool			IsItemWearable()							{ return false; }
	virtual bool			IsSkillCastable()							{ return ( IsActable() && !IsUsingSkill() ); }
	virtual bool			IsMagicCastable()							{ return false; }

	// DB 쿼리 관련
	void					DBQuery( struct GameDBManager::DBProc *pWork );
	void					onEndQuery();

	// Non-usable virtual functions(Should not be called)
	virtual void					AddExp( __int64 exp, __int64 jp, bool bApplyStamina = true )	{ assert( 0 ); }
	virtual void					SetLevel( int lv )										{ assert( 0 ); }
	virtual int						GetLevel() const										{ assert( 0 ); return 0; }
	virtual int						GetJobLevel() const										{ assert( 0 ); return 0; }
	virtual int						GetStatId() const										{ assert( 0 ); return 0; }
	virtual int						GetJobId() const										{ assert( 0 ); return 0; }
	virtual void					SetPrevJobLevel( int nDepth, int jl )					{ assert( 0 ); }
	virtual int						GetPrevJobLevel( int nDepth ) const						{ assert( 0 ); return 0; }
	virtual void					SetPrevJobId( int nDepth, int jid )						{ assert( 0 ); }
	virtual int						GetPrevJobId( int nDepth ) const						{ assert( 0 ); return 0; }
	virtual void					SetJobLevel( int jl )									{ assert( 0 ); }
	virtual int						GetJobDepth() const										{ assert( 0 ); return 0; }
	virtual __int64					GetJobPoint() const										{ assert( 0 ); return 0; }
	virtual __int64					GetTotalJobPoint() const								{ assert( 0 ); return 0; }
	virtual int						GetTalentPoint() const									{ assert( 0 ); return 0; }
	virtual void					SetTalentPoint()										{ assert( 0 ); }
	virtual int						GetItemChance() const									{ assert( 0 ); return 0; }
	virtual bool					IsUsingBow() const										{ assert( 0 ); return false; }
	virtual bool					IsUsingCrossBow() const									{ assert( 0 ); return false; }
	virtual float					GetBattleLevel() const									{ assert( 0 ); return 0.0f; }
	virtual const __int64			GetBulletCount() const									{ assert( 0 ); return 0; }
	virtual struct StructItem*		GetWearedItem( ItemBase::ItemWearType idx ) const		{ assert( 0 ); return NULL; }
	virtual ItemBase::ItemWearType	GetAbsoluteWearPos( ItemBase::ItemWearType & pos )		{ assert( 0 ); return ItemBase::WEAR_NONE; }
	virtual bool					TranslateWearPosition( ItemBase::ItemWearType & pos, struct StructItem* pItem, std::vector< int > * vpOverlappItemList = NULL )			{ assert( 0 ); return false; }

	virtual AR_TIME					GetNextAttackableTime() const							{ assert( 0 ); return 0; }
	virtual void					SetNextAttackableTime( AR_TIME t )						{ assert( 0 ); }

	virtual bool					StartAttack( AR_HANDLE target, bool bNeedFastReaction = true )	{ assert( 0 ); return false; }
	virtual void					CancelAttack()											{ assert( 0 ); }
	virtual void					EndAttack()												{ assert( 0 ); }

	virtual void					onAttack( StructCreature * pTarget, int nDamage )		{ assert( 0 ); }
	virtual int						onDamage( StructCreature * pFrom, Elemental::Type elementalType, DamageType damageType, int nDamage, bool bCritical )	{ assert( 0 ); return 0; }
	virtual bool					IsBattleMode() const									{ assert( 0 ); return false; }

	virtual const CreatureStat &	GetBaseStat() const										{ assert( 0 ); return StructCreature::GetBaseStat(); }

	virtual const c_fixed10			GetFCM() const											{ assert( 0 ); return 1; }
	virtual int						GetMoveSpeed() const									{ assert( 0 ); return 100; }

	// 스킬 관련
	virtual int						GetAllSkillTP() const									{ assert( 0 ); return 0; }

protected:

	// ArScheduler 에서 StructPet 를 지우기 위해 존재함.
	// alloc/free 메커니즘에 관한 확실한 이해 전에는 수정 말것!
	virtual bool			ProcDelete();

	virtual void			onChangeProperty( const std::string & strKey, const char *data );

	virtual const char*		getClassName()	{ return "StructPet"; }

	// { Pet AI
	virtual void			onProcess( int nThreadIdx );
	void					processWalk( AR_TIME t );
	// }

	StructPet( const char *szDummy )
	{
		m_pMaster = NULL;
		m_pItem = NULL;

		m_pContentInfo = NULL;

		m_bIsSummoned = false;

		m_hHandle = 0;
		m_nSID = 0;
		memset( m_szName, 0, sizeof( m_szName ) );
		m_bNameChanged = false;

		m_nLastProcessTime = 0;

		m_nShovelingStatus = SHOVELING_STATUS_IDLE;

#ifdef _MEM_USAGE_DEBUG
		XSEH::IncreaseAllocCount( "StructPet" );
#endif
	};	// bind property 전용
	StructPet( AR_HANDLE handle, unsigned idx );
	virtual	~StructPet();
	
	// Non-usable virtual functions(Should not be called)
	virtual void			applyState( StructState & state )							{ assert( 0 ); }
	virtual void			applyStateAmplify( StructState & state )					{ assert( 0 ); }

	virtual void			incParameter( const int nBitset, const c_fixed10 & fValue, const bool bStat )			{ assert( 0 ); }
	virtual void			ampParameter( const int nBitset, const c_fixed10 & fValue, const bool bStat )			{ assert( 0 ); }
	virtual void			incParameter2( const int nBitset, const c_fixed10 & fValue )							{ assert( 0 ); }
	virtual void			ampParameter2( const int nBitset, const c_fixed10 & fValue )							{ assert( 0 ); }

	virtual unsigned short	putonItem( ItemBase::ItemWearType pos, struct StructItem * pItem )						{ assert( 0 ); return 0; }
	virtual unsigned short	putoffItem( ItemBase::ItemWearType pos )												{ assert( 0 ); return 0; }

	virtual void			calcAttribute( CreatureAttributeServer * pAttribute )									{ assert( 0 ); }
	virtual void			applyStatByState()																		{ assert( 0 ); }
	virtual void			amplifyStatByState()																	{ assert( 0 ); }

	virtual void			applyJobLevelBonus()																	{ assert( 0 ); }
	virtual void			applyPassiveSkillEffect()																{ assert( 0 ); }
	virtual void			applyPassiveSkillEffect( struct StructSkill * pSkill )									{ assert( 0 ); }
	virtual void			applyPassiveSkillAmplifyEffect()														{ assert( 0 ); }
	virtual void			applyPassiveSkillAmplifyEffect( struct StructSkill * pSkill )							{ assert( 0 ); }

	virtual void			onItemWearEffect( struct StructItem* pItem, bool bIsBaseVar, int type, c_fixed10 var1, c_fixed10 var2, float fRatio )	{ assert( 0 ); }

	virtual void			onBeforeCalculateStat()																	{ assert( 0 ); }
	virtual void			onAfterApplyStat()																		{ assert( 0 ); }
	virtual void			onApplyStat()																			{ assert( 0 ); }
	virtual void			onAfterCalculateAttributeByStat()														{ assert( 0 ); }
	virtual void			onModifyStatAndAttribute()																{ assert( 0 ); }
	virtual	void			onCompleteCalculateStat()																{ assert( 0 ); }

	virtual void			onAfterAddState( StructState & state )													{ assert( 0 ); }
	virtual void			onAfterRemoveState( StructState & state, const bool bByDead = false )					{ assert( 0 ); }
	virtual void			onUpdateState( StructState & state, bool bIsExpire = false )							{ assert( 0 ); }

	virtual void			onCantAttack( AR_HANDLE target, AR_TIME t )												{ assert( 0 ); }
	virtual void			onDead( StructCreature *pFrom, bool decreaseEXPOnDead = true )							{ assert( 0 ); }
	virtual void			onJobLevelUp()																			{ assert( 0 ); }
	virtual void			onJPChange()																			{ assert( 0 ); }
	virtual void			onExpChange()																			{ assert( 0 ); }
	virtual void			onEnergyChange()																		{ assert( 0 ); }
	virtual void			onRegisterSkill( int skill_uid, int skill_id, int skill_level, bool is_new_skill )		{ assert( 0 ); }
	virtual void			onStatChange()																			{ assert( 0 ); }
	virtual void			onAttributeChange()																		{ assert( 0 ); }
	virtual void			onHPChange( int nPrevHP )																{ assert( 0 ); }
	virtual	void			onMPChange( int nPrevMP )																{ assert( 0 ); }
	virtual void			onSPChange()																			{ assert( 0 ); }

	// 스킬/직업 초기화 관련
	virtual __int64			getJPAfterSkillReset()																	{ assert( 0 ); return 0; }
	virtual int				getTPAfterSkillReset()																	{ assert( 0 ); return 0; }
	virtual void			onResetSkill( const int jobDepth )														{ assert( 0 ); }

	struct StructPlayer*				m_pMaster;
	struct StructItem*					m_pItem;

	struct PetBase*						m_pContentInfo;

	bool								m_bIsSummoned;

	AR_HANDLE							m_hHandle;
	int									m_nSID;
	char								m_szName[19];
	bool								m_bNameChanged;

	AR_TIME								m_nLastProcessTime;

	SHOVELING_STATUS					m_nShovelingStatus;

	XCriticalSection							m_bQueryLock;
	std::list< struct GameDBManager::DBProc* >	m_lQueryList;
};
