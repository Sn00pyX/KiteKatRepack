#pragma once

#include <vector>

#include <mmo/ArObject.h>
#include <mmo/ArType.h>
#include <mmo/ArMove.h>

#include "StructBase.h"
#include "SkillBase.h"
#include "StructCreature.h"


int EnumSkillTargetsAndCalcDamage( const ArPosition & _OriginalPos, unsigned char layer, const ArPosition & _TargetPos, bool bTargetOrigin, const float fEffectLength, const int nRegionType, const float fRegionProperty, const int nOriginalDamage, const bool bIncludeOriginalPos, StructCreature * pCaster, const int nDistributeType, const int nTargetMax, /*out*/ std::vector< StructCreature * > & vTargetList, bool bEnemyOnly = true );
int EnumChainSkillTargetsAndCalcDamage( const float fEffectLength, const float fChainLength, const int nChainType, const bool bIncludeTarget, StructCreature * pCaster, StructCreature * & pInitialTarget, const int nChainMax, /*out*/ std::vector< StructCreature * > & vTargetList, const int nTargetType );
inline void AddSkillDamageResult( std::vector< SkillResult > * pvList, unsigned char type, char damage_type, const StructCreature::_DAMAGE_INFO & damage_info, AR_HANDLE handle );
inline void AddSkillResult( std::vector< SkillResult > * pvList, bool bIsSuccess, int nSuccessType = 0, AR_HANDLE handle = NULL );
inline void AddSkillDamageWithKnockBackResult( std::vector< SkillResult > * pvList, unsigned char type, char damage_type, const StructCreature::_DAMAGE_INFO & damage_info, AR_HANDLE handle, AR_UNIT x, AR_UNIT y, AR_TIME knock_back_time );
inline void AddChainSkillDamageResult( std::vector< SkillResult > * pvList, unsigned char type, char damage_type, const StructCreature::_DAMAGE_INFO & damage_info, AR_HANDLE handle, AR_HANDLE from_handle );


struct StructSkill
{
	enum SKILL_STATUS
	{
		STATUS_IDLE	= 0,
		STATUS_CAST	= 1,
		STATUS_FIRE	= 2,
		STATUS_COMPLETE = 3,
	};

	static StructSkill *	AllocSkill( struct StructCreature * pOwner, int uid, int id );
	void					FreeSkill();

	StructSkill( struct StructCreature* pOwner, int uid, int id );
	StructSkill( const StructSkill & rhs );
	virtual ~StructSkill();

	int				GetSkillUID() const						{ return m_nSkillUID; }
	bool			IsPassiveSkill() const					{ return GetSkillBase()->IsPassive();	}
	void			SetEnhance( int enhance )				{ m_nEnhance = enhance; }
	int				GetEnhance() const						{ return m_nEnhance; }	// 강화 
	int				GetSkillId() const						{ return m_pSkillBase->GetID();	}		// 스킬 ID
	int				GetToggleGroup() const					{ return m_pSkillBase->GetToggleGroup(); }
	int				GetBaseSkillLevel() const				{ return m_nSkillLevel;	}						// 기본 스킬 레벨
	int				GetCurrentSkillLevel() const			{ return m_nSkillLevel+m_nSkillLevelAdd+GetAddedSkillLevel(); }	// 효과적용된 스킬 레벨
	int				GetAddedSkillLevel() const				{ return m_pOwner->GetAddedSkillLevel( this ); }
	void			SetBaseSkillLevel( int l )				{ m_nSkillLevel = l;	}			
	int				GetRequestedSkillLevel() const			{ return m_nRequestedSkillLevel; }
	void			SetRequestedSkillLevel( int nLevel )	{ m_nRequestedSkillLevel = ( nLevel > GetCurrentSkillLevel() ? GetCurrentSkillLevel() : nLevel ); }
	const bool		IsRandomSkill() const;

//	inline 			c_fixed10 GetVar( int idx ) const	{ return GetVar( idx ); }

	inline	const struct SkillBase*	GetSkillBase() const	{ return m_pSkillBase; }

	const bool		Cancel();
	AR_TIME			GetAuraMPDecTime()					{ return m_nAuraMPDecTime; }
	void			SetAuraMPDecTime( AR_TIME nTime )	{ m_nAuraMPDecTime = nTime; }
	AR_TIME			GetAuraRefreshTime()				{ return m_nAuraRefreshTime; }
	void			SetAuraRefreshTime( AR_TIME nTime )	{ m_nAuraRefreshTime = nTime; }

	bool			CheckCoolTime( AR_TIME t ) const;			// 시전가능하면 true 리턴

	AR_TIME			GetSkillCoolTime() const;	// 한번 쓰고 기다려야 하는 시간	
	void			SetRemainCoolTime( AR_TIME t )			{ m_nNextCoolTime = std::max( (AR_TIME) 0, GetArTime() + t ); }
	AR_TIME			GetRemainCoolTime( AR_TIME t ) const	{ return m_nNextCoolTime > t ? m_nNextCoolTime - t : 0; }
	void			SetDBCoolTime( AR_TIME t )				{ m_nDBCoolTime = t; }
	AR_TIME			GetDBCoolTime() const					{ return m_nDBCoolTime; }

	void			onDamage( int nDamage );

	AR_HANDLE		GetTargetHandle() const		{ return m_hTarget; }	// 스킬 발사 대상
	ArPosition		GetTargetPosition() const	{ return m_targetPos; }
	AR_TIME			GetCastingTime() const		{ return m_nFireTime - m_nCastTime; }
	AR_TIME			GetOriginalCastingDelay() const	{ return m_nCastingDelay; }

	SkillCostModifier&	GetCostModifier() { return m_CostModifier; }

	void			SetEnhanceSkill( StructSkill* skill ) { assert( m_Modifier == NULL || m_Modifier == skill ); m_Modifier = skill; }

	int				Cast( int nSkillLevel, AR_HANDLE handle, const ArPosition & pos, unsigned char layer, AR_HANDLE hCastItem = 0 );
	void			ProcSkill();
	bool			ProcAura();
	void			ApplyHateAndHavokFromSkillResult( StructCreature* source, StructCreature* target, const SkillResult& result );

	static void		SendSkillCastFailMessage( ArObject * pObject, AR_HANDLE caster, AR_HANDLE target, unsigned short skill_id, char skill_lv, const ArPosition & pos, int error_code );

protected:
	void			Init();
	void			bindSkillInfo( int skill_id );

	void			assembleMessage( struct TS_SC_SKILL *pResultMsg, int buf_len, int nType, int cost_hp, int cost_mp );
	void			sendSkillMessage( int nType );
	void			broadcastSkillMessage( int rx, int ry, unsigned char layer, int cost_hp, int cost_mp, int nType );
	void			broadcastSkillMessage( int rx1, int ry1, int rx2, int ry2, unsigned char layer, int cost_hp, int cost_mp, int nType );
	void			broadcastSkillMessage( int rx, int ry, unsigned int range, unsigned char layer, int cost_hp, int cost_mp, int nType );

private:
	int							m_nEnhance;
	int							m_nSkillUID;
	int							m_nSkillId;
	int							m_nSkillLevel;
	int							m_nSkillLevelAdd;
	int							m_nRequestedSkillLevel;
	struct SkillBase*			m_pSkillBase;

	bool						m_bNeedUpdateToDB;
	ArPosition					m_targetPos;
	unsigned char				m_targetLayer;

	AR_HANDLE					m_hTarget;

	AR_TIME						m_nCastingDelay;
	AR_TIME						m_nCastTime;		// 캐스팅 시작한 시각
	AR_TIME						m_nFireTime;		// 스킬이 시전될 시각
	AR_TIME						m_nNextCoolTime;	// 다음 스킬 시전 가능 시각
	AR_TIME						m_nDBCoolTime;		// DB에 저장 되어있던 쿨타임
	AR_TIME						m_nAuraMPDecTime;	// 최근 오오라 사용으로 인한 MP 소진 시각
	AR_TIME						m_nAuraRefreshTime;	// 최근 오라 리프레쉬 했던 시간
	struct StructCreature*		m_pOwner;
	SKILL_STATUS				m_Status;
	bool						m_bMultiple;		// 흐음. 디비선에서 해결 할 수 있을 것 같은데.-ㅅ-;
	AR_HANDLE					m_hCastItem;		// 스킬 캐스팅이 아이템에 의해서 되었을 경우 캐스팅에 사용된 아이템 핸들

	int							m_nErrorCode;		// 메시지 보내기 위한 변수들
	AR_UNIT						m_fRange;
	int							m_nCurrentFire;		// 스킬 연타 횟수중 몇번을 연타 했는가?
	int							m_nTotalFire;		// 스킬 연타 횟수
	int							m_nTargetCount;		// 범위 스킬의 경우 맞은 애들 수
	int							m_nFireCount;		// 한방 연타의 경우 연타 횟수

	ArPosition					m_RushPos;			// 대쉬 스킬로 이동할 위치
	float						m_fRushFace;		// 대쉬 스킬 후 바라볼 방향
	int							m_nRushDamage;		// 대쉬 스킬로 들어갈 추가 데미지

	SkillCostModifier			m_CostModifier;

	StructSkill*				m_Modifier;			// 이 스킬에 영향을 주는 스킬

	std::vector< SkillResult >	m_vResultList;

	void FireSkill( struct StructCreature * pTarget, bool bIsSuccess );

	int CalculateMPCost( const SkillCostModifier& modifier );
	int CalculateHPCost( const SkillCostModifier& modifier );

	friend struct StructSkillProp;

protected:
	void	process_target( AR_TIME t, struct SKILL_TARGET_FUNCTOR & functor, struct StructCreature * pTarget );

	bool	AFFECT_RUSH_OLD( struct StructCreature * pTarget, float * pfRushDistance, ArPosition * pRushPos, float * pface );
	int		AFFECT_KNOCK_BACK( struct StructCreature * pTarget, float fRange, AR_TIME knock_back_time );

	bool	PHYSICAL_DAMAGE_RUSH( struct StructCreature* pTarget, int *pnAdditionalDamage );

	void	ADD_REGION_STATE( struct StructCreature * pTarget );
	void	ADD_STATE_BY_SELF_COST( struct StructCreature * pTarget );
	void	ADD_REGION_STATE_BY_SELF_COST( struct StructCreature * pTarget );
	void	ADD_STATE_STEP_BY_STEP( struct StructCreature* pTarget );
	// 물리 스킬들
	void	PHYSICAL_DIRECTIONAL_DAMAGE( struct StructCreature * pTarget );
	void	SINGLE_PHYSICAL_DAMAGE_T1( struct StructCreature * pTarget );
	void	SINGLE_PHYSICAL_DAMAGE_T2( struct StructCreature * pTarget );
	void	SINGLE_PHYSICAL_DAMAGE_T2_ADD_ENERGY( struct StructCreature * pTarget );
	void	MULTIPLE_PHYSICAL_DAMAGE_T1( struct StructCreature * pTarget );
	void	MULTIPLE_PHYSICAL_DAMAGE_T2( struct StructCreature * pTarget );
	void	SINGLE_PHYSICAL_DAMAGE_T3( struct StructCreature * pTarget );
	void	SINGLE_PHYSICAL_DAMAGE_ABSORB( struct StructCreature * pTarget );
	void	PHYSICAL_SINGLE_REGION_DAMAGE_OLD( struct StructCreature* pTarget );
	void	PHYSICAL_MULTIPLE_REGION_DAMAGE_OLD( struct StructCreature* pTarget );
	void	PHYSICAL_MULTIPLE_SPECIAL_REGION_DAMAGE( struct StructCreature* pTarget );
	void	PHYSICAL_SPECIAL_REGION_DAMAGE( struct StructCreature* pTarget );
	void	MULTIPLE_PHYSICAL_DAMAGE_T3( struct StructCreature* pTarget );
	void	MULTIPLE_PHYSICAL_DAMAGE_T4( struct StructCreature * pTarget );

	// 3월 23일부터 신규 정리 유형(기존 유형 대체로 추가됨)
	bool	RUSH( struct StructCreature* pTarget, float fSpeed );
	bool	AFFECT_RUSH( struct StructCreature * pTarget, float * pfRushDistance, ArPosition * pRushPos, float * pface, float fSpeed );

	void	PHYSICAL_SINGLE_DAMAGE( struct StructCreature * pTarget );
	void	PHYSICAL_SINGLE_DAMAGE_ABSORB( struct StructCreature * pTarget );
	void	PHYSICAL_SINGLE_DAMAGE_ADD_ENERGY( struct StructCreature * pTarget );
	void	PHYSICAL_SINGLE_REGION_DAMAGE( struct StructCreature* pTarget );
	void	PHYSICAL_REALTIME_MULTIPLE_DAMAGE( struct StructCreature* pTarget );
	void	PHYSICAL_REALTIME_MULTIPLE_REGION_DAMAGE( struct StructCreature* pTarget );
	void	PHYSICAL_MULTIPLE_DAMAGE( struct StructCreature* pTarget );
	void	PHYSICAL_MULTIPLE_DAMAGE_TRIPLE_ATTACK( struct StructCreature* pTarget );
	void	PHYSICAL_MULTIPLE_REGION_DAMAGE( struct StructCreature* pTarget );
	void	PHYSICAL_SINGLE_SPECIAL_REGION_DAMAGE( struct StructCreature* pTarget );

	// 마법 스킬들
	void	SINGLE_MAGICAL_DAMAGE_T1( struct StructCreature* pTarget );
	void	SINGLE_MAGICAL_DAMAGE_T2( struct StructCreature* pTarget );
	void	MULTIPLE_MAGICAL_DAMAGE_T1( struct StructCreature* pTarget );
	void	MULTIPLE_MAGICAL_DAMAGE_T2( struct StructCreature* pTarget );
	void	MULTIPLE_MAGICAL_DAMAGE_T3( struct StructCreature* pTarget );
	void	MULTIPLE_MAGICAL_DAMAGE_AT_ONCE( struct StructCreature* pTarget );
	void	MAGIC_SINGLE_REGION_DAMAGE_OLD( struct StructCreature* pTarget );
	void	MAGIC_MULTIPLE_REGION_DAMAGE_OLD( struct StructCreature* pTarget );
	void	MAGIC_MULTIPLE_REGION_DAMAGE_T2( struct StructCreature* pTarget );
	void	MAGIC_SPECIAL_REGION_DAMAGE_OLD( struct StructCreature* pTarget );
	void	MAGIC_ABSORB_DAMAGE( struct StructCreature* pTarget );

	void	SINGLE_MAGICAL_DAMAGE( struct StructCreature* pTarget );
	void	MULTIPLE_MAGICAL_DAMAGE( struct StructCreature* pTarget );
	void	SINGLE_MAGICAL_DAMAGE_OR_DEATH( struct StructCreature* pTarget );
	void	SINGLE_MAGICAL_DAMAGE_WITH_ABSORB( struct StructCreature* pTarget );
	void	ADD_HP_MP_BY_ABSORB_HP_MP( struct StructCreature* pTarget );
	void	SINGLE_MAGICAL_TARGET_HP_PERCENT_DAMAGE( struct StructCreature* pTarget );
	void	SINGLE_MAGICAL_MANABURN( struct StructCreature* pTarget );
	void	SINGLE_DAMAGE_BY_CONSUMING_TARGETS_STATE( struct StructCreature* pTarget );

	void	MAGIC_SINGLE_REGION_DAMAGE( struct StructCreature* pTarget );
	void	MAGIC_SPECIAL_REGION_DAMAGE( struct StructCreature* pTarget );
	void	SINGLE_PHYSICAL_DAMAGE_WITH_SHIELD( struct StructCreature* pTarget );
	void	MAGIC_MULTIPLE_REGION_DAMAGE( struct StructCreature* pTarget );
	void	MAGIC_MULTIPLE_REGION_DAMAGE_AT_ONCE( struct StructCreature* pTarget );
	void	MAGIC_SINGLE_REGION_PERCENT_DAMAGE( struct StructCreature* pTarget );
	void	MAGIC_SINGLE_REGION_DAMAGE_BY_SUMMON_DEAD( struct StructCreature* pTarget );

	void	PHYSICAL_CHAIN_DAMAGE( struct StructCreature* pTarget );
	void	MAGIC_CHAIN_DAMAGE( struct StructCreature* pTarget );
	void	CHAIN_HEAL( struct StructCreature* pTarget );

	// TODO : 작명 센스가 구리다. 수정하자.
	void	PHYSICAL_SINGLE_DAMAGE_DEMINISHED_HP_MP( struct StructCreature* pTarget );
	void	PHYSICAL_SINGLE_DAMAGE_PROP_REMAIN_MP( struct StructCreature* pTarget );
	void	INC_SKILL_COOL_TIME( struct StructCreature* pTarget );
	void	AMP_SKILL_COOL_TIME( struct StructCreature* pTarget );
	void	RESIST_HARMFUL_STATE();
	void    REPLENISH_ENERGY_HP_MP();
	void	INCREASE_ENERGY_UNCONSUMPTION_RATE();

	// 기타 스킬들
	void	REGION_TAUNT( struct StructCreature* pTarget );
	void	TAUNT( struct StructCreature* pTarget );
	void	REMOVE_HATE( struct StructCreature* pTarget );
	void	REGION_REMOVE_HATE( struct StructCreature* pTarget );
	void	SKILL_RESURRECTION( struct StructCreature* pTarget );
	void	SKILL_RESURRECTION_WITH_RECOVER( struct StructCreature* pTarget );
	void	SKILL_LOTTO();
	void	SKILL_ADD_HP_MP( struct StructCreature* pTarget );
	void	SKILL_ADD_REGION_HP_MP( struct StructCreature* pTarget );
	void	SKILL_ADD_REGION_HP( struct StructCreature* pTarget );
	void	SKILL_ADD_REGION_MP( struct StructCreature* pTarget );

	void	CREATE_ITEM(  struct StructCreature* pTarget, bool bSuccess );
	void	ACTIVATE_FIELD_PROP();
	void	REGION_HEAL_BY_FIELD_PROP();
	void	MAKE_AREA_EFFECT_PROP_BY_FIELD_PROP( bool bIsTrap );

	void	CREATURE_TAMING( struct StructCreature* pTarget );
	void	PET_TAMING( struct StructCreature* pTarget );

	void	SHOVELING();

	void	TOWN_PORTAL();
	const unsigned short	IsInstanceGameEnterableOwner() const;
	void	INSTANCE_GAME_ENTER();
	void	INSTANCE_GAME_EXIT();

	const int				GetTargetHuntaholicID() const;
	const unsigned short	IsHuntaholicLobbyEnterableOwner( const int nHuntaholicID ) const;
	void					WARP_TO_HUNTAHOLIC_LOBBY();

	void	RETURN_FEATHER();
	void	RETURN_BACK_FEATHER();

	void	TOGGLE_AURA( struct StructCreature* pTarget );
	void	MAKE_AREA_EFFECT_PROP( struct StructCreature* pTarget, bool bIsTrap );
	void	CORPSE_ABSORB( struct StructCreature* pTarget );
	void	CORPSE_EXPLOSION( struct StructCreature* pTarget );
	void	ADD_ENERGY();

	void	CASTING_CANCEL_WITH_ADD_STATE( struct StructCreature* pTarget );

	void	RESPAWN_NEAR_MONSTER();
	void	RESPAWN_MONSTER();

	void	Summon();
	void	UnSummon();

	void	UnSummonAndAddState();

public:

	enum
	{
		SKILL_UID_ITEM_SKILL			= -1,
		SKILL_UID_PROP_SKILL			= -2,
		SKILL_UID_MONSTER_SKILL			= -3,
		SKILL_UID_SUMMON_SKILL			= -4,
		SKILL_UID_PET_SKILL				= -5,
		SKILL_UID_SYSTEM_SKILL			= -6,
	};

	enum
	{
		SKILL_INCREASE_ENERGY				= 1082,	// 기공운용술
		SKILL_DUAL_SWORD_EXPERT				= 1181,	// 듀얼소드 익스퍼트
		SKILL_ARMOR_MASTERY					= 1201,	// 아머 마스터리
		SKILL_LIFE_OF_DARKNESS				= 1033,	// 라이프 오브 다크니스

		SKILL_CREATURE_CONTROL				= 1801,	// 크리처 컨트롤
		SKILL_CREATURE_MASTERY				= 1811,	// 크리처 마스터리
		SKILL_TECHNICAL_CREATURE_CONTROL	= 1881,	// 테크니컬 크리쳐 컨트롤
		SKILL_ADV_WEAPON_EXPERT				= 1022,	// 전문 무기 숙련
		SKILL_GAIA_ARMOR_MASTERY			= 1202,	// 방어술 연마

		SKILL_RAISE_EXP						= 11002,	// 레이즈 EXP
		SKILL_RAISE_JP						= 11003,	// 레이즈 JP

		SKILL_GAIA_FORCE_SAVING				= 2631,

		SKILL_ITEM_RESURRECTION_SCROLL		= 6001, // 부활
		SKILL_ITEM_REGENERATION_SCROLL		= 6002, // 재생
		SKILL_ITEM_HEALING_SCROLL			= 6003, // 치유
		SKILL_ITEM_MANA_RECOVERY_SCROLL		= 6004, // 마나 회복
		SKILL_ITEM_ANTIDOTE_SCROLL			= 6005,	// 해독
		SKILL_ITEM_RECHARGE_SCROLL			= 6006, // 리차지
		SKILL_TOWN_PORTAL					= 6007,	// 타운 포탈
		SKILL_RETURN						= 3802, // 스킬로 사용하는 타운 포탈
		SKILL_FORCE_CHIP					= 6008,	// 포스칩
		SKILL_SOUL_CHIP						= 6009, // 소울칩
		SKILL_LUNA_CHIP						= 6010, // 루나칩

		SKILL_ITEM_PERFECT_CREATURE_RESURRECTION_SCROLL		= 6013,	// 완전한 크리처 부활 스크롤
		SKILL_ITEM_PIECE_OF_STRENGTH		= 6014,	// 힘의 조각
		SKILL_ITEM_PIECE_OF_VITALITY		= 6015,	// 체력의 조각
		SKILL_ITEM_PIECE_OF_DEXTERITY		= 6016,	// 집중의 조각
		SKILL_ITEM_PIECE_OF_AGILITY			= 6017,	// 민첩의 조각
		SKILL_ITEM_PIECE_OF_INTELLIGENCE	= 6018,	// 지능의 조각
		SKILL_ITEM_PIECE_OF_MENTALITY		= 6019,	// 정신의 조각

		SKILL_RETURN_FEATHER				= 6020,
		SKILL_RETURN_BACK_FEATHER			= 6021,

		SKILL_FIRE_BOMB_PHYSICAL			= 64809,
		SKILL_FIRE_BOMB_MAGICAL				= 64810,

		// 회복 간격이 서로 다른 별도의 스킬 추가 ㅜㅡ;
		SKILL_ITEM_REGENERATION_SCROLL_LV1	= 6022,
		SKILL_ITEM_REGENERATION_SCROLL_LV2	= 6023,
		SKILL_ITEM_REGENERATION_SCROLL_LV3	= 6024,
		SKILL_ITEM_REGENERATION_SCROLL_LV4	= 6025,
		SKILL_ITEM_REGENERATION_SCROLL_LV5	= 6026,
		SKILL_ITEM_REGENERATION_SCROLL_LV6	= 6027,
		SKILL_ITEM_REGENERATION_SCROLL_LV7	= 6028,
		SKILL_ITEM_REGENERATION_SCROLL_LV8	= 6029,
		SKILL_ITEM_HEALING_SCROLL_LV1		= 6030,
		SKILL_ITEM_HEALING_SCROLL_LV2		= 6031,
		SKILL_ITEM_HEALING_SCROLL_LV3		= 6032,
		SKILL_ITEM_HEALING_SCROLL_LV4		= 6033,
		SKILL_ITEM_HEALING_SCROLL_LV5		= 6034,
		SKILL_ITEM_HEALING_SCROLL_LV6		= 6035,
		SKILL_ITEM_HEALING_SCROLL_LV7		= 6036,
		SKILL_ITEM_HEALING_SCROLL_LV8		= 6037,
		SKILL_ITEM_MANA_RECOVERY_SCROLL_LV1	= 6038,
		SKILL_ITEM_MANA_RECOVERY_SCROLL_LV2	= 6039,
		SKILL_ITEM_MANA_RECOVERY_SCROLL_LV3	= 6040,
		SKILL_ITEM_MANA_RECOVERY_SCROLL_LV4	= 6041,
		SKILL_ITEM_MANA_RECOVERY_SCROLL_LV5	= 6042,
		SKILL_ITEM_MANA_RECOVERY_SCROLL_LV6	= 6043,
		SKILL_ITEM_MANA_RECOVERY_SCROLL_LV7	= 6044,
		SKILL_ITEM_MANA_RECOVERY_SCROLL_LV8	= 6045,
		SKILL_CALL_BLACK_PINE_TEA			= 6046,

		SKILL_ITEM_SUMMON_SPEED_UP_SCROLL_LV1 = 6047,
		SKILL_ITEM_SUMMON_SPEED_UP_SCROLL_LV2 = 6048, 
		SKILL_ITEM_SUMMON_SPEED_UP_SCROLL_LV3 = 6049,
		
		SKILL_ITEM_ALTERED_PIECE_OF_STRENGTH			= 6061,	// 변조된 힘의 조각
		SKILL_ITEM_ALTERED_PIECE_OF_VITALITY			= 6062,	// 변조된 체력의 조각
		SKILL_ITEM_ALTERED_PIECE_OF_DEXTERITY			= 6063,	// 변조된 집중의 조각
		SKILL_ITEM_ALTERED_PIECE_OF_AGILITY				= 6064,	// 변조된 민첩의 조각
		SKILL_ITEM_ALTERED_PIECE_OF_INTELLIGENCE		= 6065,	// 변조된 지능의 조각
		SKILL_ITEM_ALTERED_PIECE_OF_MENTALITY			= 6066,	// 변조된 정신의 조각
		SKILL_ITEM_ALTERED_PIECE_OF_STRENGTH_QUEST		= 6067,	// 변조된 힘의 조각<퀘스트용>
		SKILL_ITEM_ALTERED_PIECE_OF_VITALITY_QUEST		= 6068,	// 변조된 체력의 조각<퀘스트용>
		SKILL_ITEM_ALTERED_PIECE_OF_DEXTERITY_QUEST		= 6069,	// 변조된 집중의 조각<퀘스트용>
		SKILL_ITEM_ALTERED_PIECE_OF_AGILITY_QUEST		= 6070,	// 변조된 민첩의 조각<퀘스트용>
		SKILL_ITEM_ALTERED_PIECE_OF_INTELLIGENCE_QUEST	= 6071,	// 변조된 지능의 조각<퀘스트용>
		SKILL_ITEM_ALTERED_PIECE_OF_MENTALITY_QUEST		= 6072,	// 변조된 정신의 조각<퀘스트용>

		SKILL_ITEM_ALTERED_MAX_LIFE_PIECE				= 6079,	// 변조된 활력의 생명력 조각
		SKILL_ITEM_ALTERED_MAX_MANA_PIECE				= 6080,	// 변조된 활력의 마나 조각
		SKILL_ITEM_ALTERED_AGILENESS_PIECE				= 6081,	// 변조된 기민함의 조각
		SKILL_ITEM_ALTERED_GUARDIAN_PIECE				= 6082,	// 변조된 수호자의 조각
		SKILL_ITEM_ALTERED_QUICKNESS_PIECE				= 6083,	// 변조된 신속함의 조각
		SKILL_ITEM_ALTERED_IMPREGNABLENESS_PIECE		= 6084,	// 변조된 철벽의 조각
		SKILL_ITEM_ALTERED_CAREFULNESS_PIECE			= 6085,	// 변조된 주시의 조각
		SKILL_ITEM_ALTERED_FATALNESS_PIECE				= 6086,	// 변조된 일격의 조각
		SKILL_ITEM_ALTERED_MANA_PIECE					= 6087,	// 변조된 마나의 조각
		SKILL_ITEM_ALTERED_SHARPNESS_PIECE				= 6088,	// 변조된 예리한 조각
		SKILL_ITEM_ALTERED_SPELL_PIECE					= 6089,	// 변조된 마력의 조각
		SKILL_ITEM_ALTERED_BRILLIANCE_PIECE				= 6090,	// 변조된 영롱한 조각
		SKILL_ITEM_ALTERED_INSIGHT_PIECE				= 6091,	// 변조된 통찰의 조각

		SKILL_COLLECTING					= 6901,
		SKILL_MINING						= 6902,
		SKILL_OPERATING						= 6903,
		SKILL_ACTIVATING					= 6904,
		SKILL_OPERATE_DEVICE				= 6905,
		SKILL_OPERATE_DUNGEON_CORE			= 6906,
		SKILL_OPERATE_EXPLORATION			= 6909,		// 프랍 스킬: 탐색

		SKILL_TREE_OF_HEALING_TYPE_A		= 64806,	// 베어로드 회복 프랍(지속형)
		SKILL_TREE_OF_HEALING_TYPE_B		= 64807,	// 베어로드 회복 프랍(순간형)

		SKILL_SHOVELING						= 6907,		// 펫 스킬 삽질
		SKILL_PET_TAMING					= 6908, 	// 펫 테이밍

		SKILL_CREATURE_TAMING				= 4003,		// 크리쳐 테이밍
		SKILL_ITEM_CREATURE_TAMING_SCROLL	= 4004,		// 아이템용 크리처 테이밍 스크롤

		SKILL_THROW_SMALL_SNOWBALL			= 10009,	// 작은 눈덩이 던지기(중국 오픈 이벤트 관련)
		SKILL_THROW_BIG_SNOWBALL			= 10010,	// 큰 눈덩이 던지기(중국 오픈 이벤트 관련)
		SKILL_THROW_MARBLE					= 10014,	// 구슬 던지기(데스매치 용)
		SKILL_THROW_RED_TOMATO				= 10015,	// 잘 익은 토마토<토마토 축제>
		SKILL_THROW_GREEN_TOMATO			= 10016,	// 덜 익은 토마토<토마토 축제>

		SKILL_CREATURE_RIDING				= 11001,	// 크리처 라이딩

		SKILL_UNIT_EXPERT_LV2				= 12001,
		SKILL_UNIT_EXPERT_LV3				= 12002,
		SKILL_UNIT_EXPERT_LV4				= 12003,
		SKILL_UNIT_EXPERT_LV5				= 12004,
		SKILL_UNIT_EXPERT_LV6				= 12005,

		SKILL_AMORY_UNIT					= 14001,

		SKILL_RULER_OF_TIME					= 31324,	// 시간의 지배자<둠세이어(version:7.3)>

		SKILL_FRIENDSHIP_OF_CROWN			= 47027,	// 광대의 친화력(크리처 강화 확률 증가)

		SKILL_GRACE							= 50401,	// 은총<공통 추가스킬(version:7.3)>

		SKILL_TWIN_BLADE_EXPERT				= 61010,	// 트윈 블레이드 익스퍼트(트윈 대거 + 듀얼 소드)
		SKILL_TWIN_AXE_EXPERT				= 61015,	// 쌍도끼 연마
		SKILL_TWIN_CROSSBOW_EXPERT			= 1157,		


		SKILL_NEW_YEAR_CHAMPAGNE			= 64808,	// 신년 이벤트용 샴페인 병 아이템 스킬

		SKILL_NAMUIR_LEAF_POISON			= 64813,	// 유럽 이벤트: 나뮈르 잎 중독
		SKILL_NAMUIR_RIND_BLEEDING			= 64814,	// 유럽 이벤트: 나뮈르 껍질 출혈

		SKILL_TREE_OF_HEALING_ON_DEATHMATCH	= 64815, // 회복의 나무 데스매치 전용
		SKILL_RANKED_DEATHMATCH_ENTER		= 64816, // UI로 동작하는 일반 데스매치 입장
		SKILL_FREED_DEATHMATCH_ENTER		= 64817, // UI로 동작하는 자유 데스매치 입장
		SKILL_WARP_TO_HUNTAHOLIC_LOBBY		= 64818, // UI로 동작하는 헌터홀릭 입장
		SKILL_INSTANCE_GAME_EXIT			= 64827, // UI로 동작하는 인스턴스 게임 퇴장

		SKILL_ITEM_RESPAWN_SCROLL_RANDOMLY1			= 65060,	// 랜덤 몬스터 소환 주문서1
		SKILL_ITEM_RESPAWN_SCROLL_WITH_DIFF_CODE1	= 65061,	// 레벨별 몬스터 소환 주문서1
		SKILL_ITEM_RESPAWN_SCROLL_WITH_DIFF_CODE2	= 65062,	// 레벨별 몬스터 소환 주문서2
		SKILL_ITEM_RESPAWN_SCROLL_WITH_DIFF_CODE3	= 65089,	// 레벨별 몬스터 소환 주문서3 (Epic8.3)
		SKILL_ITEM_RESPAWN_SCROLL_WITH_DIFF_CODE4	= 65090,	// 레벨별 몬스터 소환 주문서4 (Epic8.3)
		SKILL_ITEM_RESPAWN_SCROLL_WITH_DIFF_CODE5	= 65091,	// 레벨별 몬스터 소환 주문서5 (Epic8.3)
		SKILL_ITEM_RESPAWN_SCROLL_WITH_DIFF_CODE6	= 65092,	// 레벨별 몬스터 소환 주문서6 (Epic8.3)
		SKILL_ITEM_RESPAWN_SCROLL_WITH_DIFF_CODE7	= 65093,	// 레벨별 몬스터 소환 주문서7 (Epic8.3)
		SKILL_ITEM_RESPAWN_SCROLL_WITH_DIFF_CODE8	= 65094,	// 레벨별 몬스터 소환 주문서8 (Epic9.1)
		SKILL_PROP_RESPAWN1							= 65063,	// 프랍용 몬스터 소환 스킬

		SKILL_ITEM_REGENERATION_SCROLL_OF_VITALITY	= 65103,	// 활력의 재생 주문서
		SKILL_ITEM_MANA_SCROLL_OF_VITALITY			= 65104,	// 활력의 마나 주문서
		SKILL_ITEM_HEALING_SCROLL_OF_VITALITY		= 65105,	// 활력의 치유 주문서

		SKILL_ITEM_SPELL_BREAKER_SCROLL		= 9627,
		SKILL_ITEM_STONECURSE_SCROLL		= 9628,
		SKILL_ITEM_REGION_STONECURSE_SCROLL	= 9629,
	};

	
public:
	// 이하 SkillBase에 접근하는 프록시 함수들
	// Proxy 클래스를 따로 만들어서 경유시키는 형태를 생각했지만 수정해야 할 분량이 답이 안 나오는 수준인 관계로 -_-;
	// 일단은 StructSkill에 노출시킨다. 추후 유틸리티 클래스로 추출해보는 것을 고려할 것
	char IsNeedTarget() const
	{
		return OverridedValue< char, &SkillBase::IsNeedTarget >();
	}
	int GetCastCancelType() const
	{
		return OverridedValue< int, &SkillBase::GetCastCancelType >();
	}
	int GetCastCancelLevel() const
	{
		return OverridedValue< int, &SkillBase::GetCastCancelLevel >();
	}
	int GetCastRange() const 
	{
		return EnhancedValue< int, &SkillBase::GetCastRange >();
	}
	int GetValidRange()	const
	{
		return EnhancedValue< int, &SkillBase::GetValidRange >();
	}
	AR_UNIT GetFireRange() const
	{
		return EnhancedValue< AR_UNIT, &SkillBase::GetFireRange >();
	}
	int GetCostEXP( int skill_lv, int enhance ) const 
	{
		return EnhancedValue< int, int, int, &SkillBase::GetCostEXP >( skill_lv, enhance );
	}
	int GetCostJP( int skill_lv, int enhance ) const 
	{
		return EnhancedValue< int, int, int, &SkillBase::GetCostJP >( skill_lv, enhance );
	}
	int GetCostItemCode() const 
	{
		return OverridedValue< int, &SkillBase::GetCostItemCode >();
	}
	const __int64 GetCostItemCount( int skill_lv ) const 
	{
		return EnhancedValue< const __int64, int, &SkillBase::GetCostItemCount >( skill_lv );
	}
	int GetCostHP( int skill_lv ) const 
	{
		return EnhancedValue< int, int, &SkillBase::GetCostHP >( skill_lv );
	}
	float GetCostHPPercent( int skill_lv ) const 
	{
		return EnhancedValue< float, int, &SkillBase::GetCostHPPercent >( skill_lv );
	}
	int GetCostMP( int skill_lv, int enhance ) const 
	{
		return EnhancedValue< int, int, int, &SkillBase::GetCostMP >( skill_lv, enhance );
	}
	float GetCostMPPercent( int skill_lv ) const 
	{
		return EnhancedValue< float, int, &SkillBase::GetCostMPPercent >( skill_lv );
	}
	c_fixed10 GetCostEnergy( int skill_lv ) const 
	{
		return EnhancedValue< c_fixed10, int, &SkillBase::GetCostEnergy >( skill_lv );
	}
	int GetNeedLevel() const 
	{
		return EnhancedValue< int, &SkillBase::GetNeedLevel >();
	}
	int GetNeedHP() const 
	{
		return EnhancedValue< int, &SkillBase::GetNeedHP >();
	}
	int GetNeedMP() const 
	{
		return EnhancedValue< int, &SkillBase::GetNeedMP >();
	}
	int GetNeedStateId() const 
	{
		return OverridedValue< int, &SkillBase::GetNeedStateId >();
	}
	unsigned char GetNeedStateLevel() const 
	{
		return EnhancedValue< unsigned char, &SkillBase::GetNeedStateLevel >();
	}
	char NeedStateExhaust() const 
	{
		return OverridedValue< char, &SkillBase::NeedStateExhaust >();
	}
	bool IsUseableWeapon( ItemBase::ItemClass c ) const 
	{
		return OverridedValue< bool, ItemBase::ItemClass, &SkillBase::IsUseableWeapon >( c );
	}
	AR_TIME GetCoolTime( int skill_lv, int enhance ) const 
	{
		return AR_TIME( EnhancedValue< float, int, int, &SkillBase::GetCoolTimeAsFloat >( skill_lv, enhance ) * 100.0f );
	}
	AR_TIME GetCommonDelay() const 
	{
		return AR_TIME( EnhancedValue< float, &SkillBase::GetCommonDelayAsFloat >() * 100.0f );
	}
	AR_TIME GetCastDelay( int skill_lv, int enhance ) const 
	{
		return AR_TIME( EnhancedValue< float, int, int, &SkillBase::GetCastDelayAsFloat >( skill_lv, enhance ) * 100.0f );
	}
	char IsUsable( char nUseIndex ) const 
	{
		return OverridedValue< char, char, &SkillBase::IsUsable >( nUseIndex );
	}
	bool IsUseableOnAvatar() const 
	{
		return OverridedValue< bool, &SkillBase::IsUseableOnAvatar >();
	}
	bool IsUseableOnMonster() const 
	{
		return OverridedValue< bool, &SkillBase::IsUseableOnMonster >();
	}
	bool IsUseableOnSummon() const 
	{
		return OverridedValue< bool, &SkillBase::IsUseableOnSummon >();
	}
	bool IsLimitedAddedSkill() const 
	{
		return OverridedValue< bool, &SkillBase::IsLimitedAddedSkill >(); 
	}
//	int GetSkillTargetType() const 
//	{
//		return OverridedValue< int, &SkillBase::GetSkillTargetType >(); 
//	}
	int GetStateId() const 
	{
		// 기획팀의 요청으로 일단 원래 값만 사용
		return m_pSkillBase->GetStateId();
	}
	int GetStateLevel( int skill_lv, int enhance_lv ) const 
	{
		return EnhancedValue< int, int, int, &SkillBase::GetStateLevel >( skill_lv, enhance_lv ); 
	}
	int GetStateSecond( int skill_lv, int enhance_lv ) const 
	{
		return EnhancedValue< int, int, int, &SkillBase::GetStateSecond >( skill_lv, enhance_lv ); 
	}
	int GetProbabilityOnHit( int slv ) const 
	{
		return EnhancedValue< int, int, &SkillBase::GetProbabilityOnHit >( slv ); 
	}
	int GetHitBonus( int enhance, int level_diff ) const 
	{
		return EnhancedValue< int, int, int, &SkillBase::GetHitBonus >( enhance, level_diff );
	}
	int GetCriticalBonus( int skill_lv ) const
	{
		return EnhancedValue< int, int, &SkillBase::GetCriticalBonus >( skill_lv );
	}
	int GetHatePoint( int lv, int point, int enhance ) const 
	{
		return EnhancedValue< int, int, int, int, &SkillBase::GetHatePoint >( lv, point, enhance ); 
	}
	c_fixed10 GetVar( int idx ) const
	{
		return EnhancedValue< c_fixed10, int, &SkillBase::GetVar >( idx );
	}

private:
	// - 원래 사용되던 값
	//   basic(기본값) + inc(레벨당 증감) * slv(레벨) = result
	//
	// - 의도
	//   basic -> basic + mod_basic * mod_slv
	//   inc -> inc + mod_inc * mod_slv
	//
	// - 강화 스킬 적용 이후
	//   basic + mod_basic * mod_slv + (inc + mod_inc * mod_slv) * slv
	// = basic + mod_basic * mod_slv + inc * slv + mod_inc * mod_slv * slv
	// = basic + inc * slv + mod_basic * mod_slv + mod_inc * mod_slv * slv
	// = result + (mod_basic + mod_inc * slv) * mod_slv
	// = result + mod_result * mod_slv
	//
	// 둘 다 linear한 함수라 결합 법칙이 성립하므로 위와 같이 변형하여 쓸 수 있다
	//
	// 수식 계산법이 non-linear하게 변하는 경우 아래 함수들은 무용지물이 되니 참고할 것
	// 
	// std::bind를 못 쓰니 슬프다. 중복 코드를 제거할 수가 업ㅂ네...
	template < typename ReturnType, ReturnType ( SkillBase::*Func )() const >
	ReturnType EnhancedValue() const
	{
		ReturnType value = ( m_pSkillBase->*Func )();
		
		if ( m_Modifier != NULL )
		{
			ReturnType modifier = ( m_Modifier->m_pSkillBase->*Func )();
			int modifierSkillLevel = m_Modifier->GetCurrentSkillLevel();
			return value + modifier * modifierSkillLevel;
		}
		else
		{
			return value;
		}
	}

	template < typename ReturnType, typename ArgType1,
	           ReturnType ( SkillBase::*Func )( ArgType1 ) const >
	ReturnType EnhancedValue( ArgType1 arg1 ) const
	{
		ReturnType value = ( m_pSkillBase->*Func )( arg1 );

		if ( m_Modifier != NULL )
		{
			ReturnType modifier = ( m_Modifier->m_pSkillBase->*Func )( arg1 );
			int modifierSkillLevel = m_Modifier->GetCurrentSkillLevel();
			return value + modifier * modifierSkillLevel;
		}
		else
		{
			return value;
		}
	}

	template < typename ReturnType, typename ArgType1, typename ArgType2,
	           ReturnType ( SkillBase::*Func )( ArgType1, ArgType2 ) const >
	ReturnType EnhancedValue( ArgType1 arg1, ArgType2 arg2 ) const
	{
		ReturnType value = ( m_pSkillBase->*Func )( arg1, arg2 );

		if ( m_Modifier != NULL )
		{
			ReturnType modifier = ( m_Modifier->m_pSkillBase->*Func )( arg1, arg2 );
			int modifierSkillLevel = m_Modifier->GetCurrentSkillLevel();
			return value + modifier * modifierSkillLevel;
		}
		else
		{
			return value;
		}
	}

	template < typename ReturnType, typename ArgType1, typename ArgType2, typename ArgType3,
	           ReturnType ( SkillBase::*Func )( ArgType1, ArgType2, ArgType3 ) const >
	ReturnType EnhancedValue( ArgType1 arg1, ArgType2 arg2, ArgType3 arg3 ) const
	{
		ReturnType value = ( m_pSkillBase->*Func )( arg1, arg2, arg3 );
		
		if ( m_Modifier != NULL )
		{
			ReturnType modifier = ( m_Modifier->m_pSkillBase->*Func )( arg1, arg2, arg3 );
			int modifierSkillLevel = m_Modifier->GetCurrentSkillLevel();
			return value + modifier * modifierSkillLevel;
		}
		else
		{
			return value;
		}
	}

	template < typename ReturnType, ReturnType ( SkillBase::*Func )() const >
	ReturnType OverridedValue() const
	{
		return ( m_Modifier != NULL ) ? ( m_Modifier->m_pSkillBase->*Func )() : ( m_pSkillBase->*Func )();
	}

	template < typename ReturnType, typename ArgType1,
	           ReturnType ( SkillBase::*Func )( ArgType1 ) const >
	ReturnType OverridedValue( ArgType1 arg1 ) const
	{
		return ( m_Modifier != NULL ) ? ( m_Modifier->m_pSkillBase->*Func )( arg1 ) : ( m_pSkillBase->*Func )( arg1 );
	}

	template < typename ReturnType, typename ArgType1, typename ArgType2,
	           ReturnType ( SkillBase::*Func )( ArgType1, ArgType2 ) const >
	ReturnType OverridedValue( ArgType1 arg1, ArgType2 arg2 ) const
	{
		return ( m_Modifier != NULL ) ? ( m_Modifier->m_pSkillBase->*Func )( arg1, arg2 ) : ( m_pSkillBase->*Func )( arg1, arg2 );
	}

	template < typename ReturnType, typename ArgType1, typename ArgType2, typename ArgType3,
	           ReturnType ( SkillBase::*Func )( ArgType1, ArgType2, ArgType3 ) const >
	ReturnType OverridedValue( ArgType1 arg1, ArgType2 arg2, ArgType3 arg3 ) const
	{
		return ( m_Modifier != NULL ) ? ( m_Modifier->m_pSkillBase->*Func )( arg1, arg2, arg3 ) : ( m_pSkillBase->*Func )( arg1, arg2, arg3 );
	}
};

