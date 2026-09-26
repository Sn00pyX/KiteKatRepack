#pragma once


#include <vector>
#include <map>
#include <set>


#include <toolkit/ILock.h>
#include <toolkit/c_fixed.h>
#include <toolkit/XFlag.h>
#include <dump/XExceptionHandler.h>

#include "Constant.h"
#include "ContentStruct.h"
#include "StructBase.h"
#include "StructMisc.h"
#include "CreatureBase.h"
#include "GameRule.h"


struct DamageMessage;
struct DamageCalculator;
struct CalculationResult;

const int CELL_SIZE	= 6;

struct CreatureStatServer
{
	CreatureStatServer()
		: stat_id( 0 )
		, strength( 0 )
		, vital( 0 )
		, dexterity( 0 )
		, agility( 0 )
		, intelligence( 0 )
		, mentality( 0 )
		, luck( 0 )
	{}

	CreatureStatServer( const CreatureStat & stat )
		: stat_id( stat.stat_id )
		, strength( stat.strength )
		, vital( stat.vital )
		, dexterity( stat.dexterity )
		, agility( stat.agility )
		, intelligence( stat.intelligence )
		, mentality( stat.mentality )
		, luck( stat.luck )
	{}

	const CreatureStatServer& operator+= ( const CreatureStatServer& rhs )
		{
			strength += rhs.strength;
			vital += rhs.vital;
			dexterity += rhs.dexterity;
			agility += rhs.agility;
			intelligence += rhs.intelligence;
			mentality += rhs.mentality;
			luck += rhs.luck;

			return *this;
		}

	short		stat_id;
	c_fixed10	strength;
	c_fixed10	vital;
	c_fixed10	dexterity;
	c_fixed10	agility;
	c_fixed10	intelligence;
	c_fixed10	mentality;
	c_fixed10	luck;
};

struct CreatureAttributeServer
{
	CreatureAttributeServer()
		: fCritical( 0 )
		, fCriticalPower( 0 )
		, fAttackPointRight( 0 )
		, fAttackPointLeft( 0 )
		, fDefence( 0 )
		, fBlockDefence( 0 )
		, fMagicPoint( 0 )
		, fMagicDefence( 0 )
		, fAccuracyRight( 0 )
		, fAccuracyLeft( 0 )
		, fMagicAccuracy( 0 )
		, fAvoid( 0 )
		, fMagicAvoid( 0 )
		, fBlockChance( 0 )
		, fMoveSpeed( 0 )
		, fRidingSpeed( 0 )
		, fAttackSpeed( 0 )
		, fAttackRange( 0 )
		, fMaxWeight( 0 )
		, fDamageReduce (0)
		, fDamageReducePerHP (0)
		, fCastingSpeed( 0 )
		, fCoolTimeSpeed( 0 )
		, fItemChance( 0 )
		, fHPRegenPercentage( 0 )
		, fHPRegenPoint( 0 )
		, fMPRegenPercentage( 0 )
		, fMPRegenPoint( 0 )

		, fMagicalDefIgnore( 0 )
		, fMagicalDefIgnoreRatio( 0 )
		, fPhysicalDefIgnore( 0 )
		, fPhysicalDefIgnoreRatio( 0 )
		, fMagicalPenetration( 0 )
		, fMagicalPenetrationRatio( 0 )
		, fPhysicalPenetration( 0 )
		, fPhysicalPenetrationRatio( 0 )

		, fAttackSpeedRight( 0 )
		, fAttackSpeedLeft( 0 )
		, fDoubleAttackRatio( 0 )
		, fStunResistance( 0 )
		, fMoveSpeedDecreaseResistance( 0 )
		, fHPAdd( 0 )
		, fMPAdd( 0 )
		, fHPAddByItem( 0 )
		, fMPAddByItem( 0 )
	{}

	CreatureAttributeServer( const CreatureAttribute & attr )
		: fCritical( attr.nCritical )
		, fCriticalPower( attr.nCriticalPower )
		, fAttackPointRight( attr.nAttackPointRight )
		, fAttackPointLeft( attr.nAttackPointLeft )
		, fDefence( attr.nDefence )
		, fBlockDefence( attr.nBlockDefence )
		, fMagicPoint( attr.nMagicPoint )
		, fMagicDefence( attr.nMagicDefence )
		, fAccuracyRight( attr.nAccuracyRight )
		, fAccuracyLeft( attr.nAccuracyLeft )
		, fMagicAccuracy( attr.nMagicAccuracy )
		, fAvoid( attr.nAvoid )
		, fMagicAvoid( attr.nMagicAvoid )
		, fBlockChance( attr.nBlockChance )
		, fMoveSpeed( attr.nMoveSpeed )
		, fAttackSpeed( attr.nAttackSpeed )
		, fAttackRange( attr.nAttackRange )
		, fMaxWeight( attr.nMaxWeight )
		//, fDamageReduce( attr.n)
		, fCastingSpeed( attr.nCastingSpeed )
		, fCoolTimeSpeed( attr.nCoolTimeSpeed )
		, fItemChance( attr.nItemChance )
		, fHPRegenPercentage( attr.nHPRegenPercentage )
		, fHPRegenPoint( attr.nHPRegenPoint )
		, fMPRegenPercentage( attr.nMPRegenPercentage )
		, fMPRegenPoint( attr.nMPRegenPoint )

		, fPerfectBlock( attr.nPerfectBlock )			
		, fMagicalDefIgnore( attr.nMagicalDefIgnore )		
		, fMagicalDefIgnoreRatio( attr.nMagicalDefIgnoreRatio )		
		, fPhysicalDefIgnore( attr.nPhysicalDefIgnore )	
		, fPhysicalDefIgnoreRatio( attr.nPhysicalDefIgnoreRatio )		
#ifndef USE_CLIENT97
		, fMagicalPenetration( attr.nMagicalPenetration )
		, fMagicalPenetrationRatio( attr.nMagicalPenetrationRatio )		
		, fPhysicalPenetration( attr.nPhysicalPenetration )
		, fPhysicalPenetrationRatio( attr.nPhysicalPenetrationRatio )		
#endif
		, fAttackSpeedRight( 0 )
		, fAttackSpeedLeft( 0 )
		, fDoubleAttackRatio( 0 )
		, fStunResistance( 0 )
		, fMoveSpeedDecreaseResistance( 0 )
		, fHPAdd( 0 )
		, fMPAdd( 0 )
		, fHPAddByItem( 0 )
		, fMPAddByItem( 0 )
		, fRidingSpeed( 0 )
	{}

	const CreatureAttributeServer& operator+= ( const CreatureAttributeServer& rhs )
		{
			// 외부 요인에 따라 합산이 되어선 안 될 변수는 부득이하게 주석 처리
			fCritical += rhs.fCritical;
			fCriticalPower += rhs.fCriticalPower;
			fAttackPointRight += rhs.fAttackPointRight;
			//fAttackPointLeft += rhs.fAttackPointLeft;
			fDefence += rhs.fDefence;
			fBlockDefence += rhs.fBlockDefence;
			fMagicPoint += rhs.fMagicPoint;
			fMagicDefence += rhs.fMagicDefence;
			fAccuracyRight += rhs.fAccuracyRight;
			//fAccuracyLeft += rhs.fAccuracyLeft;
			fMagicAccuracy += rhs.fMagicAccuracy;
			fAvoid += rhs.fAvoid;
			fMagicAvoid += rhs.fMagicAvoid;
			fBlockChance += rhs.fBlockChance;
			//fMoveSpeed += rhs.fMoveSpeed;
			//fRidingSpeed += rhs.fRidingSpeed;
			fAttackSpeed += rhs.fAttackSpeed;
			//fAttackRange += rhs.fAttackRange;
			fMaxWeight += rhs.fMaxWeight;
			fDamageReduce += rhs.fDamageReduce;
			fDamageReducePerHP += rhs.fDamageReducePerHP;
			fCastingSpeed += rhs.fCastingSpeed;
			fCoolTimeSpeed += rhs.fCoolTimeSpeed;
			fItemChance += rhs.fItemChance;
			fHPRegenPercentage += rhs.fHPRegenPercentage;
			fHPRegenPoint += rhs.fHPRegenPoint;
			fMPRegenPercentage += rhs.fMPRegenPercentage;
			fMPRegenPoint += rhs.fMPRegenPoint;

			fPerfectBlock += rhs.fPerfectBlock;			
			fMagicalDefIgnore += rhs.fMagicalDefIgnore;		
			fMagicalDefIgnoreRatio += rhs.fMagicalDefIgnoreRatio;		
			fPhysicalDefIgnore += rhs.fPhysicalDefIgnore;	
			fPhysicalDefIgnoreRatio += rhs.fPhysicalDefIgnoreRatio;		
			fMagicalPenetration += rhs.fMagicalPenetration;
			fMagicalPenetrationRatio += rhs.fMagicalPenetrationRatio;		
			fPhysicalPenetration += rhs.fPhysicalPenetration;
			fPhysicalPenetrationRatio += rhs.fPhysicalPenetrationRatio;		

			fAttackSpeedRight += rhs.fAttackSpeedRight;
			//fAttackSpeedLeft += rhs.fAttackSpeedLeft;
			fDoubleAttackRatio += rhs.fDoubleAttackRatio;
			fStunResistance += rhs.fStunResistance;
			fMoveSpeedDecreaseResistance += rhs.fMoveSpeedDecreaseResistance;
			fHPAdd += rhs.fHPAdd;
			fMPAdd += rhs.fMPAdd;
			fHPAddByItem += rhs.fHPAddByItem;
			fMPAddByItem += rhs.fMPAddByItem;

			return *this;
		}

	c_fixed10	fCritical;				// 크리티컬
	c_fixed10	fCriticalPower;			// 크리티컬 비율	( % 단위 )
	c_fixed10	fAttackPointRight;		// 공격력(오른손. 메인)
	c_fixed10	fAttackPointLeft;		// 공격력(왼손. 보조)
	c_fixed10	fDefence;				// 방어력
	c_fixed10	fBlockDefence;			// 블록 방어력
	c_fixed10	fMagicPoint;			// 마력
	c_fixed10	fMagicDefence;			// 마법방어력
	c_fixed10	fAccuracyRight;			// 명중(오른손. 메인)
	c_fixed10	fAccuracyLeft;			// 명중(왼손. 보조)
	c_fixed10	fMagicAccuracy;			// 마명중
	c_fixed10	fAvoid;					// 회피
	c_fixed10	fMagicAvoid;			// 마법저항력
	c_fixed10	fBlockChance;			// 블럭율
	c_fixed10	fMoveSpeed;				// 이동속도
	c_fixed10	fRidingSpeed;			// 탈것(아이템, 크리처)으로 인한 이동속도
	c_fixed10	fAttackSpeed;			// 공격속도
	c_fixed10	fAttackRange;			// 사거리
	c_fixed10	fMaxWeight;				// 소지무게
	c_fixed10	fDamageReduce;
	c_fixed10	fDamageReducePerHP;
	c_fixed10	fCastingSpeed;			// 시전속도
	c_fixed10	fCoolTimeSpeed;			// 재시전속도	 
	c_fixed10	fItemChance;			// 아이템 드랍율
	c_fixed10	fHPRegenPercentage;		// HP 리젠%
	c_fixed10	fHPRegenPoint;			// HP 리젠P
	c_fixed10	fMPRegenPercentage;		// MP 리젠%
	c_fixed10	fMPRegenPoint;			// MP 리젠P

	c_fixed10	fPerfectBlock;				// 퍼펙트 블록	
	c_fixed10	fPhysicalDefIgnore;			// 물리 방어력 무시P
	c_fixed10	fPhysicalDefIgnoreRatio;	// 물리 방어력 무시%
	c_fixed10	fMagicalDefIgnore;			// 마법 방어력 무시P
	c_fixed10	fMagicalDefIgnoreRatio;		// 마법 방어력 무시%
	c_fixed10	fPhysicalPenetration;		// 물리 방어력 관통P
	c_fixed10	fPhysicalPenetrationRatio;	// 물리 방어력 관통%
	c_fixed10	fMagicalPenetration;		// 마법 방어력 관통P
	c_fixed10	fMagicalPenetrationRatio;	// 마법 방어력 관통%

	c_fixed10	fAttackSpeedRight;		// 공격속도(오른손, 메인)
	c_fixed10	fAttackSpeedLeft;		// 공격속도(왼손, 보조)
	c_fixed10	fDoubleAttackRatio;
	c_fixed10	fStunResistance;
	c_fixed10	fMoveSpeedDecreaseResistance;
	c_fixed10	fHPAdd;
	c_fixed10	fMPAdd;
	c_fixed10	fHPAddByItem;
	c_fixed10	fMPAddByItem;
};

struct CreatureStatAmplifier
{
	c_fixed10		stat_id;
	c_fixed10		strength;
	c_fixed10		vital;
	c_fixed10		dexterity;
	c_fixed10		agility;
	c_fixed10		intelligence;
	c_fixed10		mentality;
	c_fixed10		luck;
};

struct CreatureAttributeAmplifier 
{
	c_fixed10		fCritical;			// 크리티컬
	c_fixed10		fCriticalPower;		// 크리티컬 비율	( % 단위 )
	c_fixed10		fAttackPointRight;	// 공격력(오른손. 메인)
	c_fixed10		fAttackPointLeft;	// 공격력(왼손. 보조)
	c_fixed10		fDefence;			// 방어력
	c_fixed10		fBlockDefence;		// 블록 방어력
	c_fixed10		fMagicPoint;		// 마력
	c_fixed10		fMagicDefence;		// 마법방어력
	c_fixed10		fAccuracyRight;		// 명중(오른손. 메인)
	c_fixed10		fAccuracyLeft;		// 명중(왼손. 보조)
	c_fixed10		fMagicAccuracy;		// 마명중
	c_fixed10		fAvoid;				// 회피
	c_fixed10		fMagicAvoid;		// 마법저항력
	c_fixed10		fBlockChance;		// 블럭율
	c_fixed10		fMoveSpeed;			// 이동속도
	c_fixed10		fAttackSpeed;		// 공격속도
	c_fixed10		fAttackRange;		// 사거리
	c_fixed10		fMaxWeight;			// 소지무게
	c_fixed10		fCastingSpeed;		// 시전속도
	c_fixed10		fCoolTimeSpeed;		// 재시전속도	 
	c_fixed10		fItemChance;		// 아이템 드랍율
	c_fixed10		fHPRegenPercentage;	// HP 리젠%
	c_fixed10		fHPRegenPoint;		// HP 리젠P
	c_fixed10		fMPRegenPercentage;	// MP 리젠%
	c_fixed10		fMPRegenPoint;		// MP 리젠P

	c_fixed10		fPerfectBlock;				// 퍼펙트 블록
	c_fixed10		fPhysicalDefIgnore;			// 물리 방어력 무시P
	c_fixed10		fPhysicalDefIgnoreRatio;	// 물리 방어력 무시%
	c_fixed10		fMagicalDefIgnore;			// 마법 방어력 무시P
	c_fixed10		fMagicalDefIgnoreRatio;		// 마법 방어력 무시%
	c_fixed10		fPhysicalPenetration;		// 물리 방어력 관통P
	c_fixed10		fPhysicalPenetrationRatio;	// 물리 방어력 관통%
	c_fixed10		fMagicalPenetration;		// 마법 방어력 관통P
	c_fixed10		fMagicalPenetrationRatio;	// 마법 방어력 관통%

	c_fixed10		fAttackSpeedRight;		// 공격속도(오른손, 메인)
	c_fixed10		fAttackSpeedLeft;		// 공격속도(왼손, 보조)
	c_fixed10		fDoubleAttackRatio;
	c_fixed10		fStunResistance;
	c_fixed10		fMoveSpeedDecreaseResistance;
	c_fixed10		fHPAdd;
	c_fixed10		fMPAdd;
	c_fixed10		fHPAddByItem;
	c_fixed10		fMPAddByItem;
};

struct CreatureElementalResistAmplifier
{
	c_fixed10		fResist[ Elemental::TYPE_COUNT ];
};

struct ParameterForSummon
{
	CreatureStatServer			Stat;
	CreatureAttributeServer		Attribute;
	CreatureElementalResist		Resist;

	int							nMaxHP;
	int							nMaxMP;
	int							nMaxSP;
	short						nSPRegenAdd;

	c_fixed10					fAttackPointRightWithoutWeapon;
	c_fixed10					fMoveSpeed;
	c_fixed10					fAttackRange;
	
	float						fMaxHPAmplifier;
	float						fMaxMPAmplifier;
	float						fMaxSPAmplifier;
	float						fSPRegenAddAmplifier;

	CreatureAttributeAmplifier	AttributeAmplifier;
};

struct SkillCostModifier
{
	SkillCostModifier()
	{
		init();
	}

	SkillCostModifier( const SkillCostModifier& arg )
	{
		MPInc = arg.MPInc;
		MPAmp = arg.MPAmp;
		HPInc = arg.HPInc;
		HPAmp = arg.HPAmp;
	}

	SkillCostModifier( const float MPInc, const float MPAmp, const float HPInc, const float HPAmp )
	{
		init();
		add( MPInc, MPAmp, HPInc, HPAmp );
	}

	void init()
	{
		MPInc = 0.0f;
		MPAmp = 0.0f;
		HPInc = 0.0f;
		HPAmp = 0.0f;
	}

	void add( const SkillCostModifier& modifier )
	{
		add( modifier.MPInc, modifier.MPAmp, modifier.HPInc, modifier.HPAmp );
	}

	void add( const float MPInc, const float MPAmp, const float HPInc, const float HPAmp )
	{
		this->MPInc += MPInc;
		this->MPAmp += MPAmp;
		this->HPInc += HPInc;
		this->HPAmp += HPAmp;
	}

	const float calculateMPCost( const float original )
	{
		return ( original + MPInc ) * ( 1.0f + MPAmp );
	}

	const float calculateHPCost( const float original )
	{
		return ( original + HPInc ) * ( 1.0f + HPAmp );
	}

	float MPInc;
	float MPAmp;
	float HPInc;
	float HPAmp;
};

struct LearningCost
{
	LearningCost() : jp( 0 ), tp( 0 ) {}
	LearningCost( const __int64 jp, const int tp ) : jp( jp ), tp( tp ) {}
	const LearningCost& operator +=( const LearningCost& rhs )
	{
		jp += rhs.jp;
		tp += rhs.tp;
		return *this;
	}

	__int64 jp;
	int tp;
};

struct StructCreature : GameObject
{
	ITERATOR_DEFINITION( StructCreature );

	static iterator get( AR_HANDLE handle )
	{
		iterator It;
		It.set( *GameObject::get( handle ) );

		if( (*It) && !(*It)->IsCreature() )
		{
			It.set( NULL );
		}
		return It;
	}

	static void SetMaxSkillUID( int nUID );
	static void SetMaxStateUID( int nUID );
	static void SetDisabledStateList( std::vector< int > & vDisabledStateList );
	static void InitCreatureSystem();
	static void DeInitCreatureSystem();

	struct QuadTreeItem
	{
		QuadTreeItem()
		{
			pParent = NULL;
			x = 0;
			y = 0;
		}

		inline int						GetQuadX() const  { return x; }
		inline int						GetQuadY() const  { return y; }		
		bool operator==( const QuadTreeItem & rh ) const  { return pParent == rh.pParent; }
		inline void						Set( StructCreature & c )
		{
			x = getX( c.GetPos().x );
			y = getY( c.GetPos().y );
			pParent = &c;
		}
		inline void						Set( AR_UNIT x, AR_UNIT y )
		{
			this->x = getX( x );
			this->y = getY( y );
		}

		static bool						IsCanAdd( AR_UNIT x, AR_UNIT y );
		void							AddMe();
		bool							RemoveMe();

	private:

		static int getX( AR_UNIT x ) { return int(( x + CELL_SIZE/2 ) / CELL_SIZE); }
		static int getY( AR_UNIT y ) { return int(( y + CELL_SIZE/2 ) / CELL_SIZE); }

		StructCreature *pParent;
		int		x, y;
	} quadTreeItem;

	struct _DAMAGE
	{
		_DAMAGE()
		{
			nDamage = 0;
			nResistedDamage = 0;

			bCritical = false;
			bMiss = false;
			bBlock = false;
			bPerfectBlock = false;

			target_hp = 0;
		}

		int nDamage;
		int nResistedDamage;

		bool	bCritical;
		bool	bMiss;
		bool	bBlock;
		bool	bPerfectBlock;
		int		target_hp;
	};

	struct _DAMAGE_INFO : public _DAMAGE
	{
		_DAMAGE_INFO()
		{
			memset( elemental_damage, 0, sizeof( elemental_damage ) );
		}

		void SetDamage( const StructCreature::_DAMAGE & damage )
		{
			nDamage = damage.nDamage;

			bCritical = damage.bCritical;
			bMiss = damage.bMiss;
			bBlock = damage.bBlock;
			bPerfectBlock = damage.bPerfectBlock;

			target_hp = damage.target_hp;
		}

		int elemental_damage[ Elemental::TYPE_COUNT ];
	};

	struct _ATTACK_INFO : public _DAMAGE_INFO
	{
		_ATTACK_INFO()
		{
			mp_damage = 0;
			attacker_damage = 0;
			attacker_mp_damage = 0;

			target_mp = 0;
			attacker_hp = 0;
			attacker_mp = 0;
		}

		void SetDamageInfo( const _DAMAGE_INFO & damage_info )
		{
			SetDamage( damage_info );

			elemental_damage[0] = damage_info.elemental_damage[0];
			elemental_damage[1] = damage_info.elemental_damage[1];
			elemental_damage[2] = damage_info.elemental_damage[2];
			elemental_damage[3] = damage_info.elemental_damage[3];
			elemental_damage[4] = damage_info.elemental_damage[4];
			elemental_damage[5] = damage_info.elemental_damage[5];
			elemental_damage[6] = damage_info.elemental_damage[6];
		}

		int mp_damage;
		int attacker_damage;
		int attacker_mp_damage;

		int target_mp;
		int attacker_hp;
		int attacker_mp;
	};

	struct StateMod
	{
		StateMod()
		{
			Init();
		}

		void Init()
		{
			fDamage = 1.0f;
			nDamage = 0;
			fCritical = 1.0f;
			nCritical = 0;
			fHate = 1.0f;
		}

		float				fDamage;
		int					nDamage;
		float				fCritical;
		int					nCritical;
		float				fHate;
	};

	struct StateFlagChecker
	{
		StateFlagChecker( const int flag ) : flag( flag ) {}
		const bool operator() ( const StructState& state ) {
			return ( state.GetTimeType() & flag ) && !state.IsByEvent();
		}
		int flag;
	};

	StructCreature();

	bool					IsCreature() const							{ return true;				}

	virtual const char*		GetName() const								{ return "UnknownCreature";	}

	virtual int				GetSID() const								{ return -1;				}
	virtual int				GetStatId() const							{ return 200;				}
	virtual int				GetLevel() const							{ return m_nLevel;		}
	virtual void			SetLevel( int lv )							{ m_nLevel = lv;		}
	virtual int				GetMaxReachedLevel() const					{ return m_nMaxReachedLevel; }
	virtual void			SetMaxReachedLevel( int nMaxReachedLevel )	{ m_nMaxReachedLevel = nMaxReachedLevel; }
	virtual float			GetBattleLevel() const						{ return m_fBattleLevel; }
	virtual int				GetJobId() const							{ return m_nJob;		}
	virtual int				GetJobLevel() const							{ return m_nJobLevel;	}
	virtual int				GetPrevJobLevel( int nDepth ) const			{ return ( nDepth >= 0 && nDepth < GameRule::MAX_JOB_DEPTH ) ? m_nPrevJobLevel[nDepth] : 0;	}
	virtual int				GetPrevJobId( int nDepth ) const			{ return ( nDepth >= 0 && nDepth < GameRule::MAX_JOB_DEPTH ) ? m_nPrevJobId[nDepth] : 0;	}
	virtual int				GetJobDepth() const;
	virtual void			SetJobLevel( int jl )						{ m_nJobLevel = jl;	onJobLevelUp();	}
	virtual void			SetPrevJobLevel( int nDepth, int jl )		{ ( nDepth >= 0 && nDepth < GameRule::MAX_JOB_DEPTH - 1 ) ? m_nPrevJobLevel[nDepth] = jl : 0;	}
	virtual void			SetPrevJobId( int nDepth, int jid )			{ ( nDepth >= 0 && nDepth < GameRule::MAX_JOB_DEPTH - 1 ) ? m_nPrevJobId[nDepth] = jid : 0;	}
	virtual __int64			GetJobPoint() const							{ return m_nJobPoint;	}
	virtual __int64			GetTotalJobPoint() const					{ return m_nTotalJobPoint;	}	
	// 특성, 플레이어 전용이지만 확장성을 위해 사용
	virtual int				GetTalentPoint() const						{ return 0;	}
	virtual void			SetTalentPoint( int nTalentPoint )			{ return;	}

	virtual void			AddExp( __int64 exp, __int64 jp, bool bApplyStamina = true );
	__int64					GetEXP() const												{ return m_nEXP;	}
	inline __int64			GetLastDecreasedEXP()										{ return m_nLastDecreasedEXP; }
	inline void				SetLastDecreasedEXP( __int64 nLastDecreasedEXP )			{ m_nLastDecreasedEXP = nLastDecreasedEXP; }
	void					SetEXP( __int64 exp );	
	void					SetJP( __int64 jp );

	virtual unsigned long	GetSkinColor() const										{ return 0; }

	bool					IsBackOf( const StructCreature & target );	// 대상의 뒤에 있는지?
	bool					IsSideOf( const StructCreature & target );	// 대상의 옆에 있는지?
	
	int						GetHP()	const				{ return m_nHP;				}
	int						GetMaxHP() const			{ return m_nMaxHP;			}
	unsigned char			GetHPPercentage() const		{ unsigned char p = static_cast< unsigned char >( ( static_cast< float >( m_nHP ) / m_nMaxHP ) * 100 ); return ( m_nHP ? ( p ? p : 1 ) : 0 );  }
	int						GetMP()	const				{ return m_nMP;				}
	int						GetMaxMP() const			{ return m_nMaxMP;			}
	unsigned char			GetMPPercentage() const		{ unsigned char p = static_cast< unsigned char >( ( static_cast< float >( m_nMP ) / m_nMaxMP ) * 100 ); return ( m_nMP ? ( p ? p : 1 ) : 0 );  }
	int						AddHP( int hp )				{ return SetHP( hp + GetHP() ); }
	int						Heal( int hp )				{ if( IsMagicalImmune() ) return 0; AddHP( hp * m_fHealRatio + m_nAdditionalHeal ); return hp * m_fHealRatio + m_nAdditionalHeal; }
	int						HealByItem( int hp )		{ if( IsMagicalImmune() ) return 0; AddHP( hp * m_fHealRatioByItem + m_nAdditionalHealByItem ); return hp * m_fHealRatioByItem + m_nAdditionalHealByItem; }
	int						AddMP( int mp )				{ return SetMP( mp + GetMP() ); }	
	int						MPHeal( int mp )			{ if( IsMagicalImmune() ) return 0; AddMP( mp * m_fMPHealRatio + m_nAdditionalMPHeal ); return mp * m_fMPHealRatio + m_nAdditionalMPHeal; }
	int						MPHealByItem( int mp )		{ if( IsMagicalImmune() ) return 0; AddMP( mp * m_fMPHealRatioByItem + m_nAdditionalMPHealByItem ); return mp * m_fMPHealRatioByItem + m_nAdditionalMPHealByItem; }

	// TS_SC_REGEN_HPMP 메시지 방송 있음
	void					RegenFullHPMP();

	float					GetHateRatio()				{ return m_fHateRatio; }

	int						SetHP( int hp )				{ int prev_hp = m_nHP; m_nHP = hp; if( m_nHP > m_nMaxHP ) m_nHP = m_nMaxHP; if( m_nHP < 0 ) m_nHP = 0; if( prev_hp != m_nHP ) onHPChange( prev_hp ); return m_nHP; }
	int						SetMP( int mp )				{ int prev_mp = m_nMP; m_nMP = mp; if( m_nMP > m_nMaxMP ) m_nMP = m_nMaxMP; if( m_nMP < 0 ) m_nMP = 0; if( prev_mp != m_nMP ) onMPChange( prev_mp ); return m_nMP; }

	int						GetEnergyCount() const				{ return m_nEnergy; }
	int						GetMaxEnergyCount() const			{ return m_nMaxEnergy; }
	unsigned char			GetEnergyUnconsumptionRate() const	{ return m_nEnergyUnconsumptionRate; }
	void					AddEnergy( int nEnergy = 1 );
	void					RemoveEnergy( int nEnergy );


	int						GetCastKeep()				{ return m_nCastKeep;		}

	virtual bool			IsUsingBow() const			{ return false;				}
	virtual bool			IsUsingCrossBow() const		{ return false;				}
	inline bool				IsUsingDoubleWeapon() const { return m_StatusFlag.IsOn( STATUS_USING_DOUBLE_WEAPON ); }	// 이도류 여부
	virtual const __int64	GetBulletCount() const;
	bool					IsWearShield() const;
	bool					IsWearArmor() const			{ return !!GetWearedItem( ItemBase::WEAR_ARMOR ); }
	virtual bool			TranslateWearPosition( ItemBase::ItemWearType & pos, struct StructItem* pItem, std::vector< int > * vpOverlappItemList = NULL );
	virtual ItemBase::ItemWearType			GetAbsoluteWearPos( ItemBase::ItemWearType & pos )	{ if( !m_anWear[pos] ) return static_cast< ItemBase::ItemWearType >( -1 ); else return pos; }	// 실제 장착 위치 얻어오는 함수
	bool					IsWearable( ItemBase::ItemWearType pos, struct StructItem* pItem )	{ return TranslateWearPosition( pos, pItem ); }
	bool					IsWeared( AR_HANDLE ItemHandle );
	bool					IsWeared( ItemBase::ItemCode code );
	virtual struct StructItem*		GetWearedItem( ItemBase::ItemWearType idx )	const		{ if( idx < ItemBase::MAX_SPARE_ITEM_WEAR && idx >= 0 ) return m_anWear[idx]; else return NULL; }
	struct StructItem*		GetWearedItemByAbsolutePos( ItemBase::ItemWearType idx )	const		{ if( idx < ItemBase::MAX_SPARE_ITEM_WEAR && idx >= 0 ) return m_anWear[idx]; else return NULL; }
	unsigned short			Puton( ItemBase::ItemWearType pos, struct StructItem* pItem );
	unsigned short			Putoff( ItemBase::ItemWearType pos );
	void					PutonSet( struct StructItem * pItemList[] );

	// 상태 관련
	bool					IsDead() const				{ return !m_nHP;			}
	bool					IsAlive() const				{ return !IsDead();			}
	virtual bool			IsSitDown() const			{ return false; }
	AR_TIME					GetDeadTime() const			{ return m_nDeadTime;		}
	void					SetDeadTime( AR_TIME dead_time )	{ m_nDeadTime = dead_time; }

	bool					IsFirstEnter() const				{ return m_StatusFlag.IsOn( STATUS_FIRST_ENTER ); }
	void					SetFirstEnter( bool bFirstEnter )	{ m_StatusFlag.Set( STATUS_FIRST_ENTER, bFirstEnter ); }

	virtual unsigned char	GetRace() const				{ return 0;					}

	virtual AR_TIME			GetNextAttackableTime() const		{ return m_nNextAttackableTime;	}
	virtual void			SetNextAttackableTime( AR_TIME t )	{ m_nNextAttackableTime = t; }
	virtual AR_TIME			GetNextCastableTime() const			{ return m_nNextCastableTime; }
	virtual void			SetNextCastableTime( AR_TIME t )	{ m_nNextCastableTime = t; }
	void					SetMovableTime( AR_TIME t )		{ m_nMovableTime = t; }
	AR_TIME					GetMovableTime() const			{ return m_nMovableTime; }

	bool					SetPendingMove( const std::vector< ArPosition > & newPos, unsigned char speed );
	

	enum DamageType
	{
		DT_NORMAL_PHYSICAL_DAMAGE			= 0,	// 일반 공격
		DT_NORMAL_MAGICAL_DAMAGE			= 1,	// 일반 마법
		DT_NORMAL_PHYSICAL_SKILL_DAMAGE		= 2,	// 물리 스킬
		DT_ADDITIONAL_DAMAGE				= 3,	// 추가 데미지
		DT_NORMAL_PHYSICAL_LEFT_HAND_DAMAGE	= 4,	// 왼손 데미지
		DT_ADDITIONAL_LEFT_HAND_DAMAGE		= 5,	// 왼손 추가 데미지
		DT_ADDITIONAL_MAGICAL_DAMAGE		= 6,	// 추가 데미지
		DT_STATE_MAGICAL_DAMAGE				= 7,	// 지속 마법 데미지
		DT_STATE_PHYSICAL_DAMAGE			= 8,	// 지속 물리 데미지
	};

	enum DamageFlag
	{
		IGNORE_AVOID				= ( 1 << 1 ),	// 회피 무시
		IGNORE_DEFENCE				= ( 1 << 2 ),	// 방어 무시
		IGNORE_BLOCK				= ( 1 << 3 ),	// 블럭 무시
		IGNORE_CRITICAL				= ( 1 << 4 ),	// 클티 무시(클티 안뜸)
		FORCE_DOUBLE				= ( 1 << 6 ),
	};

	void Attack( StructCreature *pTarget, AR_TIME t, AR_TIME attack_interval, struct StructCreature::_ATTACK_INFO * arDamage, bool & bIsDoubleAttack );

	// 에테리얼 내구도 관련
	virtual void			ProcEtherealDurabilityConsumption( const bool bIsAttacker, const enum DamageType eDamageType, const int nDamage )		{}

	// 데미지 인포를 리턴함.;;
	
	StructCreature::_DAMAGE_INFO DealPhysicalNormalDamage( StructCreature *pFrom, int nDamage, Elemental::Type elemental_type = Elemental::TYPE_NONE, int accuracy_bonus = 0, int critical_bonus = 0, int nFlag = 0 );
	StructCreature::_DAMAGE_INFO DealPhysicalNormalLeftHandDamage( StructCreature *pFrom, int nDamage, Elemental::Type elemental_type = Elemental::TYPE_NONE, int accuracy_bonus = 0, int critical_bonus = 0, int nFlag = 0);

	StructCreature::_DAMAGE_INFO DealMagicalSkillDamage( StructCreature *pFrom, int nDamage, Elemental::Type elemental_type = Elemental::TYPE_NONE, int accuracy_bonus = 0, int critical_bonus = 0, int nFlag = 0 );
	StructCreature::_DAMAGE_INFO DealPhysicalSkillDamage( StructCreature *pFrom, int nDamage, Elemental::Type elemental_type = Elemental::TYPE_NONE, int accuracy_bonus = 0, int critical_bonus = 0, int nFlag = 0 );

	// 데미지를 리턴함
	StructCreature::_DAMAGE DealMagicalDamage( StructCreature *pFrom, int nDamage, Elemental::Type elemental_type = Elemental::TYPE_NONE, int accuracy_bonus = 0, int critical_bonus = 0, int nFlag = 0, const StateMod * damage_penalty = NULL, const StateMod * damage_advantage = NULL );
	StructCreature::_DAMAGE DealPhysicalDamage( StructCreature *pFrom, int nDamage, Elemental::Type elemental_type = Elemental::TYPE_NONE, int accuracy_bonus = 0, int critical_bonus = 0, int nFlag = 0, const StateMod * damage_penalty = NULL, const StateMod * damage_advantage = NULL );
	StructCreature::_DAMAGE DealStateMagicalDamage( StructCreature *pFrom, int nDamage, Elemental::Type elemental_type = Elemental::TYPE_NONE, int accuracy_bonus = 0, int critical_bonus = 0, int nFlag = 0, const StateMod * damage_penalty = NULL, const StateMod * damage_advantage = NULL );
	StructCreature::_DAMAGE DealPhysicalStateDamage( StructCreature *pFrom, int nDamage, Elemental::Type elemental_type = Elemental::TYPE_NONE, int accuracy_bonus = 0, int critical_bonus = 0, int nFlag = 0, const StateMod * damage_penalty = NULL, const StateMod * damage_advantage = NULL );
	StructCreature::_DAMAGE DealPhysicalLeftHandDamage( StructCreature *pFrom, int nDamage, Elemental::Type elemental_type = Elemental::TYPE_NONE, int accuracy_bonus = 0, int critical_bonus = 0, int nFlag = 0, const StateMod * damage_penalty = NULL, const StateMod * damage_advantage = NULL );
	StructCreature::_DAMAGE DealAdditionalDamage( StructCreature *pFrom, int nDamage, Elemental::Type elemental_type = Elemental::TYPE_NONE, int accuracy_bonus = 0, int critical_bonus = 0, int nFlag = 0, const StateMod * damage_penalty = NULL, const StateMod * damage_advantage = NULL );
	StructCreature::_DAMAGE DealAdditionalLeftHandDamage( StructCreature *pFrom, int nDamage, Elemental::Type elemental_type = Elemental::TYPE_NONE, int accuracy_bonus = 0, int critical_bonus = 0, int nFlag = 0, const StateMod * damage_penalty = NULL, const StateMod * damage_advantage = NULL );
	StructCreature::_DAMAGE DealAdditionalMagicalDamage( StructCreature *pFrom, int nDamage, Elemental::Type elemental_type = Elemental::TYPE_NONE, int accuracy_bonus = 0, int critical_bonus = 0, int nFlag = 0, const StateMod * damage_penalty = NULL, const StateMod * damage_advantage = NULL );
	StructCreature::_DAMAGE DealDamage( StructCreature *pFrom, int nDamage, Elemental::Type elemental_type, DamageType damageType, int accuracy_bonus, int critical_bonus, int nFlag, const StateMod * damage_penalty, const StateMod * damage_advantage );

	int			GetCriticalDamage( int damage, float critical_amp, int critical_bonus );	// 크리티컬 추가 데미지

	virtual bool		StartAttack( AR_HANDLE target, bool bNeedFastReaction = true );
	virtual void		CancelAttack();
	virtual void		EndAttack();
	bool				IsAttacking() const	{ return !!m_hEnemy; }
	AR_HANDLE			GetEnemyHandle() const	{ return m_hEnemy; }
	virtual void		onAttack( StructCreature * pTarget, int nDamage )	{}
	virtual int			onDamage( StructCreature * pFrom, Elemental::Type elementalType, DamageType damageType, int nDamage, bool bCritical, bool bDouble);
	virtual bool		IsBattleMode() const { return false; }
	void				AddHateToEnemyList( AR_HANDLE handle, int pt );
	bool				AddToEnemyList( AR_HANDLE handle );
	bool				RemoveFromEnemyList( AR_HANDLE handle );
	size_t				GetEnemyCount() { return m_vEnemyList.size(); }
	void				RemoveAllHate();

	virtual const CreatureStat &	GetBaseStat() const { static CreatureStat inst; return inst; }

	inline const CreatureStatServer &			GetStat() const					{ return m_Stat;				}
	inline const CreatureAttributeServer &		GetAttribute() const			{ return m_Attribute;			}
	inline const CreatureStatServer &			GetStatByState() const			{ return m_StatByState;			}
	inline const CreatureAttributeServer &		GetAttributeByState() const		{ return m_AttributeByState;	}

	inline int GetHPRegenPercentage()	{ return m_Attribute.fHPRegenPercentage;	}
	inline int GetHPRegenPoint()		{ return m_Attribute.fHPRegenPoint;			}
	inline int GetMPRegenPercentage()	{ return m_Attribute.fMPRegenPercentage;	}
	inline int GetMPRegenPoint()		{ return m_Attribute.fMPRegenPoint;			}
	ItemBase::ItemClass	GetWeaponClass();
	ItemBase::ItemClass GetArmorClass();

	virtual int		GetCreatureGroup() const		{ return (m_nChangingGroup == CREATURE_NONE) ? CREATURE_ETC : m_nChangingGroup;	}

	inline const c_fixed10 &		GetStrength() const				{ return m_Stat.strength;		}
	inline const c_fixed10 &		GetVital() const				{ return m_Stat.vital;			}
	inline const c_fixed10 &		GetDexterity() const			{ return m_Stat.dexterity;		}
	inline const c_fixed10 &		GetAgility() const				{ return m_Stat.agility;		}
	inline const c_fixed10 &		GetIntelligence() const			{ return m_Stat.intelligence;	}
	inline const c_fixed10 &		GetMentality() const			{ return m_Stat.mentality;		}
	inline const c_fixed10 &		GetLuck() const					{ return m_Stat.luck;			}

	virtual const c_fixed10		GetFCM() const					{ return 1;							}
	inline float				GetWeight() const				{ return m_fWeight;					}
	inline int					GetCritical() const				{ return m_Attribute.fCritical;		}	// 크리티컬
	inline int					GetCriticalPower() const		{ return m_Attribute.fCriticalPower;}	// 일단 100%로..
	inline int					GetAttackPointRight() const		{ return m_Attribute.fAttackPointRight;	}	// 오른손 공격력
	int							GetAttackPointRight( Elemental::Type type, bool bPhysical, bool bBad );
	inline int					GetAttackPointLeft() const		{ return m_Attribute.fAttackPointLeft;	}	// 왼손 공격력
	inline int					GetDefence() const				{ return m_Attribute.fDefence;		}	// 방어력
	inline int					GetMagicPoint() const			{ return m_Attribute.fMagicPoint;	}	// 마력
	int							GetMagicPoint( Elemental::Type type, bool bPhysical, bool bBad );
	inline int					GetMagicDefence() const			{ return m_Attribute.fMagicDefence;	}	// 마법방어력
	inline int					GetAccuracyRight() const		{ return m_Attribute.fAccuracyRight;}	// 명중(오른손)
	inline int					GetAccuracyLeft() const			{ return m_Attribute.fAccuracyLeft;	}	// 명중(왼손)
	inline int					GetMagicAccuracy() const		{ return m_Attribute.fMagicAccuracy;}	// 마명중
	inline int					GetAvoid() const				{ return m_Attribute.fAvoid;		}	// 회피
	inline int					GetMagicAvoid() const			{ return m_Attribute.fMagicAvoid;	}	// 마법회피
	inline int					GetBlockChance() const			{ return m_Attribute.fBlockChance;	}	// 블럭율
	inline int					GetBlockDefence() const			{ return m_Attribute.fBlockDefence;	}	// 블럭 방어력
	virtual int					GetMoveSpeed() const			{ return m_Attribute.fMoveSpeed;	}	// 이동속도
	inline int					GetRidingSpeed() const			{ return m_Attribute.fRidingSpeed;	}	// 라이딩 속도
	inline int					GetAttackSpeed() const			{ return m_Attribute.fAttackSpeed;	}	// 공격속도
	inline int					GetMaxWeight() const			{ return m_Attribute.fMaxWeight;	}	// 소지가능무게
	inline int					GetCastingSpeed() const			{ return m_Attribute.fCastingSpeed;	}	// 시전속도
	float						GetCastingMod( Elemental::Type type, bool bPhysical, bool bBad, AR_TIME nOriginalCoolTime );
	float						GetManaCostRatio( Elemental::Type type, bool bPhysical, bool bBad );
	float						GetCoolTimeMod( Elemental::Type type, bool bPhysical, bool bBad );
	float						GetCoolTimeSpeed()				{ return m_Attribute.fCoolTimeSpeed / 100.0f;	}
	virtual int					GetItemChance() const			{ return m_Attribute.fItemChance;	}
	AR_UNIT						GetDetectHideRange() const		{ return m_fDetectHideRange;		}
	float						GetCreatureCardChance() const	{ return m_fCreatureCardChance;		}
	int							GetElementalResist( Elemental::Type elemental_type ) const			{ return m_Resist.nResist[ elemental_type ];	}
	float						GetMagicalHateMod( Elemental::Type type, bool bPhysical, bool bBad );
	std::pair< float, int>		GetHateMod( int nHateModType /* 1: PhysicalSkill, 2: MagicalSkill, 3: NormalAttack */, bool bIsHarmful );

	int							GetAttackLuna() const { return m_nAttackLuna; }
	int							GetLuna() const { return m_nLuna; }

	const SkillCostModifier&    GetSkillCostModifier() const { return m_SkillCostModifier; }

	virtual float	GetScale() const				{ return GameRule::DEFAULT_HUMAN_SIZE;						}
	virtual float	GetSize() const					{ return GameRule::DEFAULT_HUMAN_SIZE;						}
	inline AR_UNIT	GetUnitSize() const				{ return GetSize() * (float) GameRule::DEFAULT_UNIT_SIZE * GetScale();	}
	
	// (2007-03-24 리뉴얼 적용)
	inline AR_TIME			GetAttackInterval() const		{ return 115 * ( 100.0f / GetAttackSpeed() ); }
	inline AR_UNIT			GetRealAttackRange() const		{ return ( m_Attribute.fAttackRange * GameRule::DEFAULT_UNIT_SIZE ) / (float)GameRule::ATTACK_RANGE_UNIT; }
	unsigned char			GetRealMoveSpeed() const;
	unsigned char			GetRealRidingSpeed() const;
	inline const c_fixed10	GetAttackPointRightWithoutWeapon() const	{ return m_fAttackPointRightWithoutWeapon; }
	const c_fixed10			GetAttackPointRightWithoutWeapon( Elemental::Type type, bool bPhysical, bool bBad ) const;
	inline const c_fixed10	GetAttackPointLeftWithoutWeapon() const		{ return m_fAttackPointLeftWithoutWeapon; }

	inline AR_TIME	GetBowAttackInterval() const	{ return GetAttackInterval() * ( 1.0f - m_fBowInterval ); }
	inline AR_TIME	GetBowInterval() const			{ return GetAttackInterval() * m_fBowInterval; }

	// 일정 초마다 정기적으로 호출됨
	virtual void	OnUpdate();


	/////////////////////////////////////////////////////////////////////////////////////////
	// 상태이상
	static int								AllocStateUID( int & nDBEnable );
	static void								DeallocStateUID( const int nUID );
	unsigned short							AddState( StructState::StateCode code, AR_HANDLE caster, int level, AR_TIME start_time, AR_TIME end_time, bool bIsAura = false, int nStateValue = 0, const char * szStateValue = "", const bool bByEvent = false, const int nAllocatedUID = 0, int nDBEnable = StructState::NOT_IN_DB );
	void									PendAddState( StructState::StateCode code, AR_HANDLE caster, int level, AR_TIME start_time, AR_TIME end_time, bool bIsAura = false, int nStateValue = 0, const char * szStateValue = "", const bool bByEvent = false );
	void									PendAddStateByItem( StructState::StateCode code, AR_HANDLE caster, int level, AR_TIME start_time, AR_TIME end_time, bool bIsAura = false, int nStateValue = 0, const char * szStateValue = "", const bool bByEvent = false );
	void									AddStateByAttack( StructCreature *pTarget, const bool bIsAttacking, const DWORD nAttackType, const Elemental::Type nElementalType );
	void									RemoveState( StructState::StateCode code, int state_level = GameRule::MAX_STATE_LEVEL, const bool bByEvent = false );	// state_level 이하는 다 제거됨
	void									RemoveStatesOnDamaged();
	template< typename Predicate >
	void									RemoveStateIf( Predicate& predicate, std::vector< StructState >* result = NULL, const bool bByDead = false );
	void									RemoveGoodState( int state_level );	// 이로운 state_level 이하 지속 효과는 다 제거됨
	void									DecreaseState( StructState::StateCode code, const unsigned char level, const bool bByEvent = false );
	template< typename Predicate >
	void									DecreaseStateIf( Predicate& predicate, const unsigned char level, std::vector< StructState >* result = NULL );
	void									PendClearStateWithFlag( const StateInfo::_ATTRIBUTE_FLAG & nStateFlag )		{ m_nPendedClearStateFlag |= static_cast< int >( nStateFlag ); }
	const std::vector< StructState > &		GetStateList() const														{ return m_vStateList; }
	const c_fixed10							GetResistStateRate( StructState::StateCode code )
	{
		std::map< StructState::StateCode, c_fixed10 >::const_iterator it = m_mapResistHarmfulState.find( code );
		return ( it != m_mapResistHarmfulState.end() ) ? (*it).second : 0;
	}
	
	struct StructState * GetState( const StructState::StateCode code );
	const struct StructState * GetState( const StructState::StateCode code ) const;

	bool ClearExpiredState( AR_TIME t = 0 );
	void RestoreRemovedStateByDead();
	inline void ClearRemovedStateByDead();

	void RemoveAllAura();
	void RemoveAllStateByDeadOrLogout();
	void RemoveAllStateByQuittingHuntaholic();
	void RemoveStateByEnteringDeathmatch();
	void RemoveAllStateByQuittingDeathmatch();
	void RemoveAllStateByQuittingBattleArena();
	
	const bool IsFeared() const			{ return m_StatusFlag.IsOn( STATUS_FEARED ); }
	const bool IsFormChanged() const	{ return m_StatusFlag.IsOn( STATUS_FORM_CHANGED ); }

	bool IsPhysicalImmune()		{ return ( GetState( StructState::SEAL ) != NULL ); }
	bool IsMagicalImmune()		{ return ( GetState( StructState::SEAL ) != NULL || GetState( StructState::SHINE_WALL ) != NULL ); }

	bool IsNeedStateNotify() const { if( m_vStateList.size() ) return true; return false; }

	virtual bool IsActable()
	{
		return ( !IsDead() && !IsFeared() && ( m_StatusFlag.IsOn( STATUS_MOVABLE ) || m_StatusFlag.IsOn( STATUS_ATTACKABLE ) || m_StatusFlag.IsOn( STATUS_SKILL_CASTABLE ) || m_StatusFlag.IsOn( STATUS_MAGIC_CASTABLE ) || m_StatusFlag.IsOn( STATUS_ITEM_USABLE ) ) );
	}

	virtual bool IsAttackable()
	{
		if( !IsActable() || IsSitDown() || IsUsingSkill() ) return false;
		return m_StatusFlag.IsOn( STATUS_ATTACKABLE );
	}

	virtual bool IsMovable()
	{
		if( IsDead() || IsSitDown() ) return false;
		if( GetMovableTime() > GetArTime() ) return false;
		if( IsUsingSkill() ) return false;

		return m_StatusFlag.IsOn( STATUS_MOVABLE );
	}

	bool IsNeedToUpdateState() const								{ return m_StatusFlag.IsOn( STATUS_NEED_TO_UPDATE_STATE ); }
	void SetNeedToUpdateState( bool bNeedToUpdateState = true )		{ m_StatusFlag.Set( STATUS_NEED_TO_UPDATE_STATE, bNeedToUpdateState ); }

	virtual bool IsKnockbackable()
	{
		return true;
	}

	virtual bool IsItemUseable()
	{
		if( !IsActable() || IsUsingSkill() ) return false;

		return m_StatusFlag.IsOn( STATUS_ITEM_USABLE );
	}

	virtual bool IsItemWearable()
	{
		if( !IsActable() ) return false;

		return true;
	}

	virtual bool IsSkillCastable()
	{
		if( !IsActable() || GetState( StructState::MUTE ) ) return false;
		if( IsSitDown() ) return false;
		if( IsUsingSkill() ) return false;
		if( IsAttacking() ) return false;
		return m_StatusFlag.IsOn( STATUS_SKILL_CASTABLE );
	}

	virtual bool IsMagicCastable()
	{
		if( !IsActable() || GetState( StructState::MUTE ) ) return false;
		if( IsSitDown() ) return false;
		if( IsUsingSkill() ) return false;
		if( IsAttacking() ) return false;
		return m_StatusFlag.IsOn( STATUS_MAGIC_CASTABLE );
	}

	virtual bool		IsEnemy( StructCreature* pTarget, bool bIncludeHiding = false );
	virtual bool		IsAlly( StructCreature* pTarget );

	// 범용적인 부활 처리, 추후에 기존의 부활들도 이 함수를 통하도록 수정
	bool Resurrect( const _CHARACTER_RESURRECTION_TYPE eResurrectType, const int nIncHP, const int nIncMP, const __int64 nRecoveryEXP, const bool bIsRestoreRemovedStateByDead );
	bool ResurrectByState();

	bool AddAimer( AR_HANDLE aimer );
	bool RemoveAimer( AR_HANDLE aimer );
	void ReleaseAimerList();

	// 스킬 관련
	struct SKILL_FUNCTOR			{ virtual void onSkill( int sid, int skill_id, int base_skill_level, int current_skill_level, AR_TIME total_cool_time, AR_TIME remain_cool_time ) {} };
	struct ADDED_SKILL_FUNCTOR		{ virtual void onSkill( int skill_id, bool restricted_to_type, char added_skill_level ) {} };
	struct STATE_FUNCTOR			{ virtual void onState( struct StructState* pState ) {} };
	struct SKILL_POINTER_FUNCTOR	{ virtual void onSkill( struct StructSkill* pSkill ) {} };

	struct StructSkill* 		GetCastSkill() { return m_pCastSkill; }
	struct StructSkill*			SetSkill( int skill_uid, int skill_id, int skill_level, AR_TIME remain_cool_time );
	void						RegisterSkill( int skill_id, int skill_level, AR_TIME remain_cool_time = 0, int nJobId = 0 );
	size_t						GetSkillCount() const { return m_vAllSkillList.size(); }
	__int64						GetAllSkillJP() const;
	__int64						GetAllJobLevelJP() const;
	virtual int					GetAllSkillTP() const { return 0; }
	virtual int					IsLearnableSkill( int nSkillID, int nSkillLevel, int *nSkillTreeID );

	enum _SKILL_RESET_METHOD
	{
		SRM_ITEM				= 0,
		SRM_SCRIPT				= 1,
		SRM_SUMMON_ENHANCE		= 2,
	};

	bool						ResetSkill( const _SKILL_RESET_METHOD eMethod, const int jobDepth = 0, bool doResetRandomSkill = false );
	int							GetBaseSkillLevel( int skill_id ) const;
	int							GetAddedSkillLevel( const StructSkill *const pSkill ) const;
	int							GetCurrentSkillLevel( int skill_id ) const;
	int							GetCurrentPassiveSkillLevel( int skill_id ) const;
	struct StructSkill*			GetCurrentPassiveSkill( int skill_id ) const;
	struct StructSkill *		GetSkill( int skill_id ) const;
	const struct StructSkill *	GetSkillByEffectType( const int nEffectTypeID ) const;
	void						AddRemainCoolTime( const int skill_id, const int nInc, const float fAmp );
	AR_TIME						GetRemainCoolTime( int skill_id ) const;
	AR_TIME						GetTotalCoolTime( int skill_id ) const;
	void						EnumSkill( SKILL_FUNCTOR & _fo ) const;
	void						EnumActiveSkill( SKILL_POINTER_FUNCTOR & _fo );
	void						EnumPassiveSkill( SKILL_POINTER_FUNCTOR & _fo );
	void						EnumAddedSkill( ADDED_SKILL_FUNCTOR & _fo ) const;
	void						EnumState( STATE_FUNCTOR & _fo );
	bool						CancelSkill();
	bool						OnCompleteSkill();
	const unsigned short		CastSkill( int nSkillId, int nSkillLevel, AR_HANDLE target_handle, const ArPosition & pos, unsigned char layer, AR_HANDLE hCastItem = 0 );
	void						FireSkill();
	bool						IsActiveAura( struct StructSkill * pSkill );
	bool						TurnOnAura( struct StructSkill * pSkill );
	bool						TurnOffAura( struct StructSkill * pSkill );
	void						ToggleAura( struct StructSkill * pSkill );
	bool						IsUsingSkill();
	void						BindSkillCard( struct StructItem *pItem, const bool bSkipDBUpdate = false );
	void						UnBindSkillCard( struct StructItem *pItem );
	StateMod *					GetMagicalSkillStatePenalty()				{ return &m_MagicalSkillStatePenalty; }
	inline void					SetTrapHandle( const AR_HANDLE handle )		{ m_hTrap = handle; }
	AR_HANDLE					GetTrapHandle();

	unsigned short				onItemUseEffect( struct StructCreature *pCaster, struct StructItem* pItem, int type, c_fixed10 var1, c_fixed10 var2, const char *szParameter = NULL );

	virtual void				onRegionChange( unsigned prev_rx, unsigned prev_ry, unsigned rx, unsigned ry ) {}

	virtual void				onRemoveFromWorld() { RemoveAllHate(); ReleaseAimerList(); }
	
	virtual void				onProcess( int nThreadIdx );
	virtual void				onHPChange( int nPrevHP );

	inline bool					IsLoginComplete()		{ return m_StatusFlag.IsOn( STATUS_LOGIN_COMPLETE ); }
	inline void					SetLoginComplete()		{ m_StatusFlag.On( STATUS_LOGIN_COMPLETE ); }

	inline bool					IsReadInfoComplete()	{ return m_StatusFlag.IsOn( STATUS_READ_INFO_COMPLETE ); }
	inline void					SetReadInfoComplete()	{ m_StatusFlag.On( STATUS_READ_INFO_COMPLETE ); }

	inline const bool			IsNeedToBroadcastStatusFlag() const							{ return m_bNeedToBroadcastStatusFlag; }
	inline void					SetNeedToBroadcastStatusFlag( const bool bNeed = true )		{ m_bNeedToBroadcastStatusFlag = bNeed; }

	inline bool					HasPendingMove()	{ return m_StatusFlag.IsOn( STATUS_MOVE_PENDED ); }
	inline void					ReleasePendingMove()	{ m_StatusFlag.Off( STATUS_MOVE_PENDED ); }
	void						ProcessAttack();
	void						ProcessAddHPMPOnCritical();

	void						SetNeedCalculateStat()	{ m_StatusFlag.On( STATUS_NEED_TO_CALCULATE_STAT ); }

	void						RemoveExhaustiveSkillStateMod( bool bPhysical, bool bHarmful, int nElementalType, AR_TIME nOriginalCastingDelay );
	void						PrepareRemoveExhaustiveSkillStateMod( bool bPhysical, bool bHarmful, int nElementalType, AR_TIME nOriginalCastingDelay );

	void						SetInvisible( bool bInvisible )		{ m_StatusFlag.Set( STATUS_INVISIBLE, bInvisible ); }
	bool						IsInvisible() const		{ return m_StatusFlag.IsOn( STATUS_INVISIBLE ); }
	void						SetInvincible( bool bInvincible ) { m_StatusFlag.Set( STATUS_INVINCIBLE, bInvincible ); }
	bool						IsInvincible() const	{ return m_StatusFlag.IsOn( STATUS_INVINCIBLE ); }
	bool						IsHiding() const		{ return m_StatusFlag.IsOn( STATUS_HIDING ); }
	virtual bool				IsVisible( StructCreature *pTarget );
	inline bool					IsProcessingReflectDamage() const			{ return m_StatusFlag.IsOn( STATUS_PROCESSING_REFELCT ); }

	int							damage( StructCreature * pFrom, int nDamage, bool decreaseEXPOnDead = true );

	// 이하 핸들러
	void						OnAttack( StructCreature *pTarget, const int nDamage, const DWORD nAttackType, const int nElementalType, const int nSkillId = 0 );
	void						OnKill( StructCreature *pTarget );
	void						OnCritical( StructCreature *pTarget, const int nDamage, const DWORD nAttackType, const int nElementalType );
	void						OnAvoid( StructCreature *pFrom, const DWORD nAttackType, const int nElementalType );
	void						OnBlock( StructCreature *pFrom, const int nDamage, const DWORD nAttackType, const int nElementalType );
	void						OnPerfectBlock( StructCreature *pFrom, const DWORD nAttackType, const int nElementalType );
	void						OnDoubleAttack( StructCreature *pTarget );

	// 강화 관련
	virtual int					GetEnhance() const				{ return 0; }

	const ParameterForSummon &	GetParameterForSummon()		{ return m_ParameterForSummon;		}
	const CreatureStatServer &		GetStatBeforeParamBasedCalc()	{ return m_StatBeforeParamBasedCalc;	}
	const CreatureAttributeServer &	GetAttrBeforeParamBasedCalc()	{ return m_AttrBeforeParamBasedCalc;	}

	c_fixed10					CalcStatSum( const int bitset, const CreatureStatServer& statBeforeCalc );
	c_fixed10					CalcStatSum( const int bitset ) { return CalcStatSum( bitset, m_Stat ); }
	c_fixed10					CalcAttributeSum( const int bitset1, const int bitset2 ) { return CalcAttributeSum( bitset1, bitset2, m_Attribute); }
	virtual c_fixed10			CalcAttributeSum( const int bitset1, const int bitset2, const CreatureAttributeServer& attrBeforeCalc );

protected:

	virtual ~StructCreature();

	friend struct DB_Login;
	friend struct DB_ReadCharacter;
	friend struct DB_ReadStorageList;

	void					CheckAndSetEnhanceSkill( StructSkill* skill );

	void					removeStateByDead();
	void					removeStateWithFlag( const int & nFlag );

	void					processSkill();
	void					processAttack();
	void					processPendingMove();
	
	// 생물체 상태 플래그 관련
	const bool				isFirstAttack() const					{ return m_StatusFlag.IsOn( STATUS_FIRST_ATTACK ); }
	const bool				isAttackStarted() const					{ return m_StatusFlag.IsOn( STATUS_ATTACK_STARTED ); }
	const bool				isMortal() const						{ return m_StatusFlag.IsOn( STATUS_MORTAL ); }
	const bool				isMoveSpeedFixed() const				{ return m_StatusFlag.IsOn( STATUS_MOVE_SPEED_FIXED ); }
	const bool				isFearMoving() const					{ return m_StatusFlag.IsOn( STATUS_MOVING_BY_FEAR ); }
	const bool				isHPRegenStopped() const				{ return m_StatusFlag.IsOn( STATUS_HP_REGEN_STOPPED ); }
	const bool				isMPRegenStopped() const				{ return m_StatusFlag.IsOn( STATUS_MP_REGEN_STOPPED ); }
	const bool				isNeedToCalculateStat() const			{ return m_StatusFlag.IsOn( STATUS_NEED_TO_CALCULATE_STAT ); }

	virtual void			onAfterAddState( StructState & state )									{ procMoveSpeedChangement(); }
	virtual void			onAfterRemoveState( StructState & state, const bool bByDead = false )	{ procMoveSpeedChangement(); }
	virtual void			procMoveSpeedChangement();

	virtual unsigned short	putonItem( ItemBase::ItemWearType pos, struct StructItem * pItem );	// insert absolute position
	virtual unsigned short	putoffItem( ItemBase::ItemWearType pos );	// insert absolute position
	static unsigned short	putoffItem( StructCreature * pCreature, ItemBase::ItemWearType pos ); // 아악. 존나 구려!!

	virtual void			onUpdateState( StructState & state, bool bIsExpire = false );
	bool					onProcAura( struct StructSkill *pSkill, int nRequestedSkillLevel );

	virtual void			onCantAttack( AR_HANDLE target, AR_TIME t )				{}
	virtual void			onDead( StructCreature *pFrom, bool decreaseEXPOnDead = true );
	virtual	void			onMPChange( int nPrevMP )							{}
	virtual void			onJobLevelUp()										{}
	virtual void			onJPChange()										{}
	virtual void			onExpChange()										{}
	virtual void			onEnergyChange()									{}
	virtual void			onRegisterSkill( int skill_uid, int skill_id, int skill_level, bool is_new_skill )	{}
	virtual void			onStatChange()										{}
	virtual void			onAttributeChange()									{}	

	// 스킬 초기화 관련
	virtual __int64			getJPAfterSkillReset()								{ return GetJobPoint() + GetAllSkillJP(); }
	virtual int				getTPAfterSkillReset()								{ return 0; }

	virtual void			onResetSkill( const int jobDepth )					{}
	void					AdjustOverflowedSkillLevel( const int remainingMaxJobDepth );
		
	template< typename SkillPredicate >
	LearningCost			RemoveSkill( const SkillPredicate& predicate );
	template< typename SkillPredicate >
	LearningCost			removeSkillFromList( const SkillPredicate& predicate );
	template< typename SkillPredicate >
	void					turnOffAuraOnSkillReset( const SkillPredicate& predicate );

	virtual void			onRemoveSkill( StructSkill* removedSkill )			{}

	const bool				UpdateSkillLevel( StructSkill* skill, int targetLevel );

	virtual void			regenHPMP( AR_TIME t );
	void					procStateDamage( AR_TIME t );
	void					procState( AR_TIME t );

	int						m_nLuna;
	int						m_nAttackLuna;
	int						m_nRace;

	int						m_nCriticalCount;

	int						m_nHP;
	int						m_nMaxHP;
	int						m_nMinHP;
	c_fixed10				m_fMinHP;
	int						m_nHPDecPart;
	int						m_nMP;
	int						m_nMaxMP;
	int						m_nMPDecPart;

	int						m_nMaxEnergy;
	int						m_nEnergy;
	int						m_nEnergyStartPos;
	AR_TIME					m_arEnergy[GameRule::ENERGY_MAX];
	AR_TIME					m_nEnergyUpkeepTime;
	unsigned char			m_nEnergyUnconsumptionRate;

	// 회복량 증폭 관련
	float					m_fHealRatio;
	float					m_fMPHealRatio;
	float					m_fHealRatioByItem;
	float					m_fMPHealRatioByItem;
	float					m_fHealRatioByRest;
	float					m_fMPHealRatioByRest;

	// 회복량 증가 관련
	int						m_nAdditionalHeal;
	int						m_nAdditionalMPHeal;
	int						m_nAdditionalHealByItem;
	int						m_nAdditionalMPHealByItem;
	int						m_nAdditionalHealByRest;
	int						m_nAdditionalMPHealByRest;

	SkillCostModifier		m_SkillCostModifier;

	// 어그로 증폭 관련
	float					m_fHateRatio;

	float					m_nCastKeep;

	int						m_nLevel;
	int						m_nMaxReachedLevel;
	float					m_fBattleLevel;
	int						m_nUnitExpertLevel;

	__int64					m_nEXP;
	__int64					m_nLastDecreasedEXP;

	struct Experience
	{
		Experience() : exp( 0 ), jp( 0 ) {}
		Experience( const __int64 exp, const __int64 jp) : exp( exp ), jp( jp ) {}
		Experience( const Experience& arg ) : exp( arg.exp ), jp( arg.jp ) {}

		const Experience operator+ ( const Experience& rhs ) const
		{
			return Experience( rhs.exp + exp, rhs.jp + jp );
		}

		const Experience& operator+= ( const Experience& rhs )
		{
			exp += rhs.exp;
			jp += rhs.jp;

			return *this;
		}

		__int64 exp;
		__int64 jp;
	};

	float					m_fEXPMod;

	int						m_nJob;	
	int						m_nJobLevel;	
	int						m_nPrevJobId[GameRule::MAX_JOB_DEPTH - 1];
	int						m_nPrevJobLevel[GameRule::MAX_JOB_DEPTH - 1];
	__int64					m_nJobPoint;
	__int64					m_nTotalJobPoint;

	int						m_nRegenHP;
	int						m_nRegenMP;
	
	float					m_fWeight;				// 현재 소지한 아이템 무게(m_Inventory.GetWeight()를 이용해
													// 필요할 때 마다 갱신시켜야 함... 왜 있을까 -_ -;?

	CreatureStatServer		m_Stat;
	CreatureStatServer		m_StatByState;
	CreatureAttributeServer	m_Attribute;
	CreatureAttributeServer	m_AttributeByState;
	CreatureElementalResist	m_Resist;

	c_fixed10				m_fAttackPointRightWithoutWeapon;
	c_fixed10				m_fAttackPointLeftWithoutWeapon;
	float					m_fBowInterval;

	c_fixed10				m_ElementalStateDamageAdder[Elemental::TYPE_COUNT];
	c_fixed10				m_ElementalStateDamageAmplifier[Elemental::TYPE_COUNT];

	// 플레이어와 소환수 간의 상호 참조형 스킬을 위한 변수
	ParameterForSummon	m_PrevParameterForSummon;
	ParameterForSummon	m_ParameterForSummon;
	float				m_fPassivePhysicalReflect;
	float				m_fPassiveMagicalReflect;

	AR_TIME				m_LastAttackTime;

	// 생물체의 상태를 Bit flag로 사용하여 메모리 사용량 감소
	enum CreatureStatus
	{
		// 내부 처리용
		STATUS_LOGIN_COMPLETE			= 0,	// 로그인 처리 완료
		STATUS_FIRST_ENTER				= 1,	// 최초 Enter 상태(Enter 메시지에 사용하여 클라에 알려줌)
		STATUS_ATTACK_STARTED			= 2,	// 공격 처리 중(공격 상태 중에 다음 공격 대기 상태가 아닌 실제 데미지처리 중인 순간)
		STATUS_FIRST_ATTACK				= 3,	// 첫 공격(타겟의 OnUpdate 호출 처리 여부 확인용)
		STATUS_MOVE_PENDED				= 4,	// 대기 중인 이동이 있음(타 액션 중이어서 이동을 못하고 대기시켜둔 상태)
		STATUS_NEED_TO_UPDATE_STATE		= 5,	// 지속효과 DB 업데이트가 필요함(소환수에만 사용되고 있음)
		STATUS_MOVING_BY_FEAR			= 6,	// 공포에 의한 이동 처리 중
		STATUS_NEED_TO_CALCULATE_STAT	= 7,	// 스텟 계산 처리 필요(CalculateStat 호출 필요성 체크)
		STATUS_PROCESSING_REFELCT		= 8,	// 반사막 효과 처리 중(쌍방 반사막 사용 시 무한 반사 현상 체크)

		// 게임 내 상태 관련
		STATUS_INVISIBLE				= 9,	// 투명(GM 명령어)
		STATUS_INVINCIBLE				= 10,	// 무적(GM 명령어)
		STATUS_HIDING					= 11,	// 은신
		//STATUS_STIGMATIZED				= 12,	// 죄의 낙인(여신의 축복을 받을 수 없음) - 기부 시스템 리뉴얼 이후로 사용하지 않음
		STATUS_MOVABLE					= 13,	// 이동 가능 여부
		STATUS_ATTACKABLE				= 14,	// 공격 가능 여부
		STATUS_SKILL_CASTABLE			= 15,	// 스킬 사용 가능
		STATUS_MAGIC_CASTABLE			= 16,	// 마법 사용 가능
		STATUS_ITEM_USABLE				= 17,	// 아이템 사용 가능
		STATUS_MORTAL					= 18,	// 피격 가능
		STATUS_FEARED					= 20,	// 공포
		STATUS_FORM_CHANGED				= 21,	// 변이
		STATUS_MOVE_SPEED_FIXED			= 22,	// 이속 고정
		STATUS_HP_REGEN_STOPPED			= 23,	// HP 리젠 중지
		STATUS_MP_REGEN_STOPPED			= 24,	// MP 리젠 중지
		STATUS_USING_DOUBLE_WEAPON		= 25,	// 이도류 사용 중
		STATUS_COMPETE_DEAD				= 26,	// 대련에 의한 사망 상태

		STATUS_READ_INFO_COMPLETE		= 27,	// 로그인 처리 뿐만 아니라 DB로부터 모든 정보를 읽어들인 상태로 호칭기록을 위해 존재한다.
	};

	XFlag< int, 1 >			m_StatusFlag;
	bool					m_bNeedToBroadcastStatusFlag;

	// 입고있는 아이템
	struct StructItem*		m_anWear[ ItemBase::MAX_ITEM_WEAR ];

	float					m_fHPRegenMod;		// HP 자연회복량 조정
	float					m_fMPRegenMod;		// MP 자연회복량 조정
	float					m_fItemMod;			// 아이템 효율


	AR_TIME					m_nMovableTime;
	AR_TIME					m_nLastUpdateTime;
	AR_TIME					m_nNextAttackableTime;
	AR_TIME					m_nNextCastableTime;
	AR_TIME					m_nLastStateProcTime;
	AR_TIME					m_nDeadTime;		// 죽은시간

	// 전투
	void					broadcastAttackMessage( StructCreature *pTarget, struct StructCreature::_ATTACK_INFO * arDamage, int tm, int delay, bool bIsDoubleAttack, bool bIsAiming = false, bool bEndAttack = false, bool bCancelAttack = false );
	void					onAttackAndSkillProcess();
	AR_HANDLE				m_hEnemy;

	XCriticalSection			m_csEnemy;
	std::vector< AR_HANDLE >	m_vEnemyList;		// Hate 관련( 현재 내가 hate를 먹고 있는 애들)

	enum					{ AM_ATTACK = 0, AM_AIMING, };
	int						m_nNextAttackMode;	

	std::vector< ArPosition >	m_PendingMovePos;
	unsigned char				m_nPendingMoveSpeed;

	AR_UNIT					m_fDetectHideRange;
	float					m_fCreatureCardChance;

	std::vector< AR_HANDLE > m_vAimerList;		// 나를 Targeting 중인 Player들 리스트

	// 상태이상
	unsigned short										m_nCurrentStateUID;
	std::vector< StructState >							m_vStateList;
	std::vector< StructState >							m_vPendStateList;
	std::vector< std::pair< AR_TIME, StructState > >	m_vPendStateListByItem;
	std::vector< StructState >							m_vStateListRemovedByDeath;
	int													m_nPendedClearStateFlag;
	typedef std::vector< StructState >::iterator		STATE_ITERATOR;
	typedef std::vector< StructState >::const_iterator	STATE_CONST_ITERATOR;

	float											m_fResistHarmfulState;	// 해로운 지속효과 저항률(자체 저항률-마법회피-과 무관하게 따로 체크)
	std::map< StructState::StateCode, c_fixed10 >	m_mapResistHarmfulState;	// 해로운 지속효과 저항률(자체 저항률-마법회피-과 무관하게 따로 체크)

	std::vector< struct _ATTACK_TAG >					m_vProcByAttack;
	std::vector< struct _ATTACK_TAG >					m_vProcByBeingAttacked;
	std::vector< struct _KILL_TAG >						m_vProcByKill;
	std::vector< struct _KILL_TAG >						m_vProcByDead;
	std::vector< struct _ATTACK_TAG >					m_vProcByCriticalAttack;
	std::vector< struct _ATTACK_TAG >					m_vProcByBeingCriticalAttacked;
	std::vector< struct _ATTACK_TAG >					m_vProcByAvoid;
	std::vector< struct _ATTACK_TAG >					m_vProcByBlock;
	std::vector< struct _ATTACK_TAG >					m_vProcByPerfectBlock;
	std::map< int, std::vector< struct _PROC_TAG > >	m_mapProcBySkillId;

	void ProcByAttack( StructCreature *pTarget, const int nDamage, const bool bIsAttacking, const DWORD nAttackType, const int nElementalType );
	void ProcBySkillId( StructCreature *pTarget, const int nDamage, const bool bIsAttacking, const int nSkillId );
	void ProcByKill( StructCreature *pTarget, const bool bIsKilling );
	void ProcByCritical( StructCreature *pTarget, const int nDamage, const bool bIsAttacking, const DWORD nAttackType, const int nElementalType );
	void ProcByAvoid( StructCreature *pFrom, const DWORD nAttackType, const int nElementalType );
	void ProcByBlock( StructCreature *pTarget, const int nDamage, const DWORD nAttackType, const int nElementalType );
	void ProcByPerfectBlock( StructCreature *pTarget, const DWORD nAttackType, const int nElementalType );

	int						m_nDoubleWeaponMasteryLevel;

	struct ExpertMod
	{
		float				fAvoid;
		float				fDamage;
	};

	ExpertMod				m_Expert[ MAX_CREATURE_TYPE_NUMBER ];

	struct ElementalSkillStateMod
	{
		ElementalSkillStateMod()
		{
			Init();
		}

		void Init()
		{
			fMagicalDamage = 1.0f;
			fPhysicalDamage = 1.0f;
			nElementalType = 0;
			fCooltime = 1.0f;
			fCastingSpeed = 1.0f;
			fHate = 1.0f;
			nMagicalAccuracy = 0;
			nPhysicalAccuracy = 0;
			nCritical = 0;
			fManaCostRatio = 1.0f;
			vExhaustiveStateCode.clear();
			nCastingSpeedApplyTime = 0;
		}

		void apply( float damage, float magicDamage, int critical, float hate, float coolTime, AR_TIME castingSpeedApplyTime,
			int castingSpeed, bool isExhaustive, StructState::StateCode exhaustiveStateCode )
		{
			fPhysicalDamage += damage;
			fMagicalDamage += magicDamage;
			nCritical += critical;
			fHate += hate;
			fCooltime += coolTime;
			nCastingSpeedApplyTime = std::max( nCastingSpeedApplyTime, castingSpeedApplyTime );
			fCastingSpeed += castingSpeed;

			if( isExhaustive )
				vExhaustiveStateCode.push_back( exhaustiveStateCode );
		}

		float						fManaCostRatio;
		float						fCooltime;
		float						fCastingSpeed;
		float						fHate;
		int							nMagicalAccuracy;
		int							nPhysicalAccuracy;
		int							nCritical;

		float						fMagicalDamage;
		float						fPhysicalDamage;
		int							nElementalType;
		AR_TIME						nCastingSpeedApplyTime;

		std::vector< StructState::StateCode >	vExhaustiveStateCodeForDelete;
		std::vector< StructState::StateCode >	vExhaustiveStateCode;
	};

	StateMod						m_NormalStateAdvantage;			// 일반 공격 보너스
	StateMod						m_RangeStateAdvantage;			// 일반 공격 보너스
	StateMod						m_NormalStatePenalty;			// 일반 피격 페널티
	StateMod						m_RangeStatePenalty;			// 일반 피격 페널티
	StateMod						m_PhysicalSkillStatePenalty;	// 물리 스킬 피격 페널티
	StateMod						m_MagicalSkillStatePenalty;		// 마법 스킬 피격 페널티
	StateMod						m_StateStatePenalty;			// 지속 효과 피격 페널티

	ElementalSkillStateMod			m_GoodPhysicalElementalSkillStateMod[Elemental::TYPE_COUNT];	// 물리 공격 보너스
	ElementalSkillStateMod			m_BadPhysicalElementalSkillStateMod[Elemental::TYPE_COUNT];	// 마법 공격 보너스
	ElementalSkillStateMod			m_GoodMagicalElementalSkillStateMod[Elemental::TYPE_COUNT];	// 마법 공격 보너스
	ElementalSkillStateMod			m_BadMagicalElementalSkillStateMod[Elemental::TYPE_COUNT];	// 마법 공격 보너스

	void							removeExhaustiveSkillStateMod( ElementalSkillStateMod *pSkillStateMod, AR_TIME nOriginalCastingDelay );

	struct HateModifier
	{
		HateModifier( int nHateModType, int nHarmfulType, float _fAmpValue, int _nIncValue )
			: fAmpValue( _fAmpValue )
			, nIncValue( _nIncValue )
		{
			bIsApplyToPhysicalSkill = false;
			bIsApplyToMagicalSkill = false;
			bIsApplyToPhysicalAttack = false;

			bIsApplyToHarmful = false;
			bIsApplyToHelpful = false;

			switch( nHateModType )
			{
			case 1:
				bIsApplyToPhysicalSkill = true;
				break;
			case 2:
				bIsApplyToMagicalSkill = true;
				break;
			case 3:
				bIsApplyToPhysicalAttack = true;
				break;
			case 99:
				bIsApplyToPhysicalSkill = true;
				bIsApplyToMagicalSkill = true;
				bIsApplyToPhysicalAttack = true;
				break;
			}

			bIsApplyToHarmful = false;
			bIsApplyToHelpful = false;

			switch( nHarmfulType )
			{
			case 0:
				bIsApplyToHelpful = true;
				break;
			case 1:
				bIsApplyToHarmful = true;
				break;
			case 99:
				bIsApplyToHelpful = true;
				bIsApplyToHarmful = true;
				break;
			}
		}

		bool	bIsApplyToPhysicalSkill;
		bool	bIsApplyToMagicalSkill;
		bool	bIsApplyToPhysicalAttack;

		bool	bIsApplyToHarmful;
		bool	bIsApplyToHelpful;

		float	fAmpValue;
		int		nIncValue;
	};

	std::vector< HateModifier >		m_vHateMod;

	struct AdditionalDamageInfo
	{
		AdditionalDamageInfo( unsigned char _ratio, Elemental::Type _require_type, Elemental::Type _type, unsigned short _nDamage, float _fDamage )
			: ratio( _ratio ), require_type( _require_type ), type( _type ), nDamage( _nDamage ), fDamage( _fDamage ) {}

		unsigned char ratio;
		Elemental::Type require_type;
		Elemental::Type type;
		unsigned short nDamage;
		float fDamage;
	};

	struct DamageReflectInfo
	{
		DamageReflectInfo( unsigned char _fire_ratio, float _range, Elemental::Type _type, unsigned short _nReflectDamage, float _fPhysicalReflectRatio, float _fPhysicalSkillReflectRatio, float _fMagicalReflectRatio, bool _bIgnoreDefence )
			: fire_ratio( _fire_ratio ), range( _range ), type( _type ), nReflectDamage( _nReflectDamage ), fPhysicalReflectRatio( _fPhysicalReflectRatio )
			, fPhysicalSkillReflectRatio( _fPhysicalSkillReflectRatio ), fMagicalReflectRatio( _fMagicalReflectRatio ), bIgnoreDefence( _bIgnoreDefence )
		{}

		unsigned char fire_ratio;
		float range;
		Elemental::Type type;
		unsigned short nReflectDamage;
		float fPhysicalReflectRatio;
		float fPhysicalSkillReflectRatio;
		float fMagicalReflectRatio;
		bool bIgnoreDefence;
	};

	struct StateReflectInfo
	{
		StateReflectInfo( StructState::StateCode _nCode, int _nLevel, AR_TIME _nDuration )
			: nCode( _nCode ), nLevel( _nLevel ), nDuration( _nDuration ) {}

		StructState::StateCode nCode;
		int nLevel;
		AR_TIME nDuration;
	};

	struct AddHPMPOnCriticalInfo
	{
		AddHPMPOnCriticalInfo( int _nAddHP, int _nAddMP, int _nActivationRate )
			: nAddHP( _nAddHP ), nAddMP( _nAddMP ), nActivationRate( _nActivationRate ) {}

		int nAddHP;
		int nAddMP;
		int nActivationRate;
	};

	struct DamageReduceInfo
	{
		DamageReduceInfo( unsigned char _ratio, float _physical_reduce, float _physical_skill_reduce, float _magical_skill_reduce,
			int _apply_creature_group_1, int _apply_creature_group_2, int _apply_creature_group_3, int _apply_creature_group_4, int _apply_creature_group_5 )
			: ratio( _ratio ), physical_reduce( _physical_reduce ), physical_skill_reduce( _physical_skill_reduce ), magical_skill_reduce( _magical_skill_reduce )
		{
			apply_creature_group_list[0] = _apply_creature_group_1;
			apply_creature_group_list[1] = _apply_creature_group_2;
			apply_creature_group_list[2] = _apply_creature_group_3;
			apply_creature_group_list[3] = _apply_creature_group_4;
			apply_creature_group_list[4] = _apply_creature_group_5;
		}

		inline bool IsAppliableCreatureGroup( int creature_group ) const
		{
			return ( apply_creature_group_list[0] == 99 || apply_creature_group_list[0] == creature_group )
				|| ( apply_creature_group_list[1] == 99 || apply_creature_group_list[1] == creature_group )
				|| ( apply_creature_group_list[2] == 99 || apply_creature_group_list[2] == creature_group )
				|| ( apply_creature_group_list[3] == 99 || apply_creature_group_list[3] == creature_group )
				|| ( apply_creature_group_list[4] == 99 || apply_creature_group_list[4] == creature_group );
		}

		unsigned char		ratio;
		float				physical_reduce;
		float				physical_skill_reduce;
		float				magical_skill_reduce;
		int					apply_creature_group_list[5];
	};

	// 추가데미지
	std::vector< AdditionalDamageInfo >		m_vNormalAdditionalDamage;
	std::vector< AdditionalDamageInfo >		m_vRangeAdditionalDamage;
	std::vector< AdditionalDamageInfo >		m_vPhysicalSkillAdditionalDamage;
	std::vector< AdditionalDamageInfo >		m_vMagicalSkillAdditionalDamage;

	std::vector< DamageReflectInfo >		m_vDamageReflectInfo;
	std::vector< StateReflectInfo >			m_vStateReflectInfo;

	std::vector< DamageReduceInfo >			m_vDamageReducePercentInfo;
	std::vector< DamageReduceInfo >			m_vDamageReduceValueInfo;

	// 계열의 변경
	int										m_nChangingGroup;

	// 크리티컬시 회복
	std::vector< AddHPMPOnCriticalInfo >	m_vAddHPMPOnCritical;

	// 사용 금지당한 스킬
	std::vector< int >						m_vInterruptedSkill;

	// 허용된 스킬
	std::vector< std::set< int > >			m_vAllowedSkill;

	// 증가된 스킬
	std::vector< std::pair< int, int > >	m_vAddedSkillBySkillId;
	std::vector< std::pair< int, int > >	m_vAddedSkillBySkillType;

	// 마나 실드에 의한 데미지 전이 비율
	float									m_fPhysicalDamageManaShieldAbsorbRatio;
	float									m_fMagicalDamageManaShieldAbsorbRatio;

	// 스킬
	struct StructSkill *					m_pCastSkill;
	std::vector< struct StructSkill * >		m_vAllSkillList;
	std::vector< struct StructSkill * >		m_vPassiveSkillList;

	std::vector< struct StructSkill * >		m_vActiveSkillList;
	struct StructSkill *					getSkill( int nSkillId ) const;
	AR_HANDLE								m_hSkillTarget;
	std::vector< std::pair<StructSkill *, int> >	m_vAura;

	std::vector< struct StructSkill * >		m_vSkillRelatedToDamaging;

	// 스킬 프랍(트랩)
	AR_HANDLE								m_hTrap;

	float									m_fSummonFCM;


private:
	// 전부 static인 까닭은 공격자와 피격자의 용이한 구분과 함께 이 부분을 차후 쉽게 분리할 수 있도록 하기 위함
	static const float CalculateDamageReduceSum( const DamageMessage& message, const std::vector< DamageReduceInfo >& reduceInfo );		

	static StructCreature::_DAMAGE DealDamage( const DamageMessage& message );
	static _DAMAGE CreateDamageStruct( const CalculationResult& result );

	static void ApplySkillToCalculation( const DamageMessage& message, DamageCalculator& calculator, StructSkill* skill );

	static void ProcessPostDamageEffect( const DamageMessage& message, const CalculationResult& result );
	static void ApplyManaShieldEffect( const DamageMessage& message, const CalculationResult& result );
	static void InterruptSpellCast( const DamageMessage& message, const CalculationResult& result );
	static void RemoveStateOnDamage( const DamageMessage& message, const CalculationResult& result );
	static void ConsumeSoulStoneDurabilityOnDamage( const DamageMessage& message, const CalculationResult& result );
	static void ConsumeEtherealDurabilityOnDamage( const DamageMessage& message, const CalculationResult& result );
	static void ReflectDamage( const DamageMessage& message, const CalculationResult& result );
	static void ProcessAdditionalDamage( const DamageMessage& sourceMessage, const DamageType additionalDamageType, const std::vector< AdditionalDamageInfo >& additionalDamage, _DAMAGE_INFO& damageInfo );

public:
	void ProvideAttackerInfo( const DamageMessage& message, DamageCalculator& calculator );
	void ProvideTargetInfo( const DamageMessage& message, DamageCalculator& calculator );

	float SetSummonFCM(float value) { m_fSummonFCM = value; return m_fSummonFCM; };
	float GetSummonFCM() const { return m_fSummonFCM; }


	// 스탯 관련 함수 및 변수들을 아래에 정리
public:
	void					CalculateStat();

protected:
	// --- 초기화 관련 루틴 ---
	void					initProc();
	virtual void			calcAttribute( CreatureAttributeServer* pAttribute );

	// --- 스탯 및 능력치 연산에 관련된 패시브 스킬들 적용하는 루틴 ---
	void					applyStatByPassiveSkill();
	void					amplifyStatByPassiveSkill();
	virtual void			applyPassiveSkillEffect();
	virtual void			applyPassiveSkillEffect( struct StructSkill * pSkill );
	virtual void			applyPassiveSkillAmplifyEffect();
	virtual void			applyPassiveSkillAmplifyEffect( struct StructSkill * pSkill );
	virtual void			calcParamBasedStat();
	virtual void			calcParamBasedAttribute();

	// --- 스탯 및 능력치 연산에 관련된 지속 효과들 적용하는 루틴 ---
	void					applyStateEffect();
	virtual void			applyState( StructState & state );
	void					applyStateAmplifyEffect();
	virtual void			applyStateAmplify( StructState & state );
	virtual void			applyStatByState();
	virtual void			amplifyStatByState();
	void					applyParamBasedStateEffect();

	// --- 스탯 및 능력치 연산에 관련된 아이템 효과를 적용하는 루틴 ---
	void					applyItemEffect();
	void					_applyEffect( struct StructItem * pItem, const std::vector< EffectInfo * > * pvEffectList, float fRatio );
	virtual void			_applyEffectForState( const EffectInfo * pEffect );
	void					_applySkillByEffect( const std::vector< EffectInfo * > * pvEffectList );
	virtual void			applyStatByItem();
	void					_applyStatByEffect( const std::vector< EffectInfo * > * pvEffectList, float fMod = 1.0f);
	virtual void			amplifyStatByItem();
	void					_amplifyStatByEffect( const std::vector< EffectInfo * > * pvEffectList );
	void					applyParamBasedItemEffect();
	void					_applyParamBasedEffect( const std::vector< EffectInfo * > * pvEffectList );
	virtual void			onItemWearEffect( struct StructItem* pItem, bool bIsBaseVar, int type, c_fixed10 var1, c_fixed10 var2, float fRatio );

	// --- 스탯 및 능력치 연산에 관련된 호칭 효과를 적용하는 루틴 ---
	virtual void			applyStatByTitle()		{}
	virtual void			amplifyStatByTitle()	{}
	virtual void			applyTitleEffect()		{}
	
	// --- 기타 예외적인 처리 ---
	void					applyDoubleWeaponEffect();
	virtual void			applyJobLevelBonus();
	void					synchronizeParameter( const int bitset, const c_fixed10 & rate );
	void					addSkill( const int id, const int added_level, const int skill_type, const bool bRace );
	void					checkAdditionalItemEffect();
		
	// --- 이하 핸들러들 ---
	virtual void			onBeforeCalculateStat();
	virtual void			onAfterApplyStat();
	virtual void			onApplyStat();
	virtual void			onAfterCalculateAttributeByStat();
	virtual void			onApplyAttributeAdjustment();
	virtual void			onModifyStatAndAttribute();
	void					onModifySkill();
	void					onModifyDetectRange();
	virtual	void			onCompleteCalculateStat();

	// --- 각종 스탯을 계산하는 유틸리티 함수들 ---
	void					accumulateStatDiffByState( const CreatureStatServer& stateStat );
	void					accumulateAttributeDiffByState( const CreatureAttributeServer& stateAttr );
	void					getAmplifiedStatByAmplifier( CreatureStatServer * pStat );
	void					getAmplifiedAttributeByAmplifier( CreatureAttributeServer * pAttribute );
	void					getAmplifiedResistByAmplifier( CreatureElementalResist * pResist );
	virtual void			incParameter( const int nBitset, const c_fixed10 & fValue, const bool bStat, ParameterForSummon *pParam = NULL );
	virtual void			ampParameter( const int nBitset, const c_fixed10 & fValue, const bool bStat );
	virtual void			incParameter2( const int nBitset, const c_fixed10 & fValue, ParameterForSummon *pParam = NULL );
	virtual void			ampParameter2( const int nBitset, const c_fixed10 & fValue );
	void					finalizeStat();
	void					onSynchronizeParameter( bool bStat );
	bool					isSkillModified( std::vector< std::pair< int, int > > * pvPrevAddedSkill, std::vector< std::pair< int, int > > * pvAddedSkill );

protected:
	// 세트 아이템 효과 적용 판단용 맵 타입 정의
	typedef std::map< int, int >			SetPartFlagMap;

	float					m_fMaxHPAmplifier;
	float					m_fMaxMPAmplifier;

	CreatureStatAmplifier				m_StatAmplifier;		// 증폭 값 계산을 한 번에 하기 위한 증폭량 누적 변수
	CreatureAttributeAmplifier			m_AttributeAmplifier;	// 증폭 값 계산을 한 번에 하기 위한 증폭량 누적 변수
	CreatureElementalResistAmplifier	m_ResistAmplifier;		// 증폭 값 계산을 한 번에 하기 위한 증폭량 누적 변수
	CreatureStatServer					m_StatBeforeParamBasedCalc;
	CreatureAttributeServer				m_AttrBeforeParamBasedCalc;	

	// 속성 이펙트 추가 성능 관련
	int						m_nElementalEffectAttackPointRight;
	int						m_nElementalEffectAttackPointLeft;
	int						m_nElementalEffectMagicPoint;

	std::vector< struct StructSkill * >		m_vAmplifyPassiveSkillList;

	// 세트 아이템, 세트 소울스톤 효과 적용을 위한 임시 버퍼
	std::vector< SetItemEffectInfo * >		m_vSetItemEffect;

	// 파라미터 동기화 ┬ 파라미터 비트셋
	//                 │
	//                 └┬ 적용률
	//                   │
	//                   └ 적용률 합산을 위한 동기화 정보의 개수 
	std::vector< std::pair< int, std::pair< c_fixed10, int > > >	m_vSynchronizedParameter;
};
