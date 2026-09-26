#pragma once

#include <mmo/ArObject.h>
#include <mmo/ArTime.h>

#include "StructSkill.h"
#include "StructBase.h"

struct StructSkillProp : public GameObject
{
	virtual ~StructSkillProp();

	AR_HANDLE					GetHandle() const	{ return m_hHandle; }

	static StructSkillProp*		Create( AR_HANDLE caster, StructSkill* pSkill, int nMagicPoint, float fHateRatio );
	bool						ProcDelete();
	
	void						onProcess( int nThreadIdx );

	virtual bool				IsSkillProp() const			{ return true; }

	int							GetSkillId()				{ return m_Skill.GetSkillId();	}
	int							GetSkillLevel()				{ return m_Skill.GetRequestedSkillLevel(); }
	AR_TIME						GetStartTime()				{ return m_Info.m_nStartTime; }
	AR_HANDLE					GetCasterHandle()			{ return m_hCaster; }

	void						PendRemove();

	struct _SKILL_PROP_INFO
	{
		AR_TIME					m_nStartTime;
		AR_TIME					m_nEndTime;
		AR_TIME					m_nInterval;
		AR_TIME					m_nLastFireTime;
	};

protected:
	void INIT_AREA_EFFECT_MAGIC_DAMAGE();
	void INIT_AREA_EFFECT_HEAL();
	void INIT_AREA_EFFECT_HEAL_BY_FIELD_PROP();

	void FIRE_AREA_EFFECT_MAGIC_DAMAGE_OLD( struct StructCreature * pCaster );
	void FIRE_AREA_EFFECT_HEAL( struct StructCreature * pCaster );
	void FIRE_AREA_EFFECT_HEAL_BY_FIELD_PROP( struct StructCreature * pCaster );

	// 스킬 프랍 유지시간, 발동 간격과 관련된 변수 초기화
	void INIT_SKILL_PROP_PARAMETER( AR_TIME nDuration, AR_TIME nInterval );

	void FIRE_AREA_EFFECT_MAGIC_DAMAGE( struct StructCreature * pCaster );
	void FIRE_AREA_EFFECT_MAGIC_DAMAGE_AND_HEAL( struct StructCreature * pCaster );
	void FIRE_AREA_EFFECT_MAGIC_DAMAGE_AND_HEAL_T2( struct StructCreature * pCaster );

	void FIRE_TRAP_DAMAGE( struct StructCreature * pCaster );
	void FIRE_TRAP_MULTIPLE_DAMAGE( struct StructCreature * pCaster );

private:

	AR_HANDLE				m_hHandle;

	StructSkillProp( AR_HANDLE caster, StructSkill* pSkill, int nMagicPoint, float fHateRatio );

	AR_HANDLE				m_hCaster;
	_SKILL_PROP_INFO		m_Info;

	StructSkill		 		m_Skill;

	bool					m_bFired;	// 트랩 유형에서 사용할 변수
	bool					m_bProcessEnd;
	bool					m_bIsRemovePended;

	int						m_nOwnerMagicPoint;
	float					m_fHateRatio;
};
