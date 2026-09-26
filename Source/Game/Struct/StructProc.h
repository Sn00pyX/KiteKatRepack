#pragma once

#include "StructCreature.h"


struct _PROC_TAG
{
	_PROC_TAG( const int _ratio, const int _min_hp, const int _max_hp, const int _target_min_hp, const int _target_max_hp, struct StructProc *_proc )
		: ratio( _ratio )
		, min_hp( _min_hp )
		, max_hp( _max_hp )
		, target_min_hp( _target_min_hp )
		, target_max_hp( _target_max_hp )
		, proc( _proc )
	{}

	const bool CheckProc( const int _hp_rate, const int _target_hp_rate ) const;

	int ratio;					// 성공률
	int min_hp;					// 최소 생명력
	int max_hp;					// 최대 생명력
	int target_min_hp;			// 대상 최소 생명력, 0이면 무시
	int target_max_hp;			// 대상 최대 생명력, 0이면 무시
	struct StructProc *proc;	// 발동 효과
};

struct _ATTACK_TAG : _PROC_TAG
{
	_ATTACK_TAG( const int _ratio, const int _min_hp, const int _max_hp, const int _target_min_hp, const int _target_max_hp, const DWORD _attack_type, const int _elemental_type, struct StructProc *_proc )
		: _PROC_TAG( _ratio, _min_hp, _max_hp, _target_min_hp, _target_max_hp, _proc )
		, attack_type( _attack_type )
		, elemental_type( _elemental_type )
	{}

	const bool CheckProcByAttack( const int _hp_rate, const int _target_hp_rate, const DWORD _attack_type, const int _elemental_type ) const;

	DWORD attack_type;			// 공격 타입(비트셋 형태의 StructState::AttackType)
	int elemental_type;			// 데미지 속성
};

struct _KILL_TAG : _PROC_TAG
{
	_KILL_TAG( const int _ratio, const int _min_hp, const int _max_hp, const int _target_min_hp, const int _target_max_hp, const int _max_diff_level, const int _min_mp_rate, struct StructProc *_proc )
		: _PROC_TAG( _ratio, _min_hp, _max_hp, 0, 0, _proc )
		, max_diff_level( _max_diff_level )
		, min_mp_rate( _min_mp_rate )
	{}

	const bool CheckProcByDead( const int _hp_rate, const int _target_hp_rate, const int _diff_level, const int _mp_rate ) const;

	int max_diff_level;
	int min_mp_rate;
};

struct StructProc
{
	enum TargetType {
		ATTACKER	= 0,
		ATTACKEE	= 1,
	};

	StructProc( const TargetType _target )
		: target( _target )
	{}

	virtual StructProc *Clone() = 0;
	virtual void Proc( StructCreature *pOwner, StructCreature *pTarget, const int nDamage, const bool bIsAttacking )  = 0;

	void * operator new( size_t nAllocSize );
	void * operator new[]( size_t nArraySize );
	void operator delete( void * pBlock );
	void operator delete[]( void * pBlock );

protected:
	TargetType target;	// 대상
};

struct StructStateProc : StructProc
{
	StructStateProc( const StructState::StateCode _code, const int _level, const AR_TIME _duration, const TargetType _target, const int _cost_mp )
		: StructProc( _target )
		, code( _code )
		, level( _level )
		, duration( _duration )
		, cost_mp( _cost_mp )
	{}

	virtual StructStateProc * Clone()		{ return new StructStateProc( *this ); }
	virtual void Proc( StructCreature *pOwner, StructCreature *pTarget, const int nDamage, const bool bIsAttacking );

protected:
	StructState::StateCode code;	// 지속효과 코드
	int	level;						// 지속효과 레벨
	AR_TIME duration;				// 지속효과 지속시간
	int cost_mp;					// 소모 MP
};

struct StructAbsorbProc : StructProc
{
	StructAbsorbProc( const TargetType _target, const float _hp_absorb_ratio, const float _mp_absorb_ratio )
		: StructProc( _target )
		, hp_absorb_ratio( _hp_absorb_ratio )
		, mp_absorb_ratio( _mp_absorb_ratio )
	{}

	virtual StructAbsorbProc * Clone()		{ return new StructAbsorbProc( *this ); }
	virtual void Proc( StructCreature *pOwner, StructCreature *pTarget, const int nDamage, const bool bIsAttacking );

protected:
	float hp_absorb_ratio; // HP 흡수율
	float mp_absorb_ratio; // MP 흡수율
};

struct StructHealProc : StructProc
{
	StructHealProc( const TargetType _target, const int _hp_inc, const int _mp_inc )
		: StructProc( _target )
		, hp_inc( _hp_inc )
		, mp_inc( _mp_inc )
	{}

	virtual StructHealProc * Clone()		{ return new StructHealProc( *this ); }
	virtual void Proc( StructCreature *pOwner, StructCreature *pTarget, const int nDamage, const bool bIsAttacking );

protected:
	int hp_inc;	// hp 회복량
	int mp_inc;	// mp 회복량
};

struct StructStealProc : StructProc
{
	StructStealProc( const TargetType _target, const int _hp_steal, const int _mp_steal )
		: StructProc( _target )
		, hp_steal( _hp_steal )
		, mp_steal( _mp_steal )
	{}

	virtual StructStealProc * Clone()		{ return new StructStealProc( *this ); }
	virtual void Proc( StructCreature *pOwner, StructCreature *pTarget, const int damage, const bool bIsAttacking );

protected:
	int hp_steal;	// hp 회복량
	int mp_steal;	// mp 회복량
};

struct StructEnergyProc : StructProc
{
	StructEnergyProc( const TargetType _target, const short _energy_count )
		: StructProc( _target )
		, energy_count( _energy_count )
	{}

	virtual StructEnergyProc * Clone()		{ return new StructEnergyProc( *this ); }
	virtual void Proc( StructCreature *pOwner, StructCreature *pTarget, const int nDamage, const bool bIsAttacking );

protected:
	int energy_count;
};

struct StructCooldownProc : StructProc
{
	StructCooldownProc( const TargetType _target, const bool _is_all_skill, const int _inc, const int _skill_id1, const int _inc1, const int _skill_id2, const int _inc2 )
		: StructProc( _target )
		, is_all_skill( _is_all_skill )
		, inc( _inc )
		, skill_id1( _skill_id1 )
		, inc1( _inc1 )
		, skill_id2( _skill_id2 )
		, inc2( _inc2 )
	{}

	virtual StructCooldownProc * Clone()	{ return new StructCooldownProc( *this ); }
	virtual void Proc( StructCreature *pOwner, StructCreature *pTarget, const int damage, const bool bIsAttacking );

protected:
	bool is_all_skill;
	int inc;
	int skill_id1;
	int inc1;
	int skill_id2;
	int inc2;
};