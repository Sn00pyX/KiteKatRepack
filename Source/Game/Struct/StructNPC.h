#pragma once

#include <vector>

#include <geometry/X2DPolygon.h>

#include "StructCreature.h"
//#include "QuestBase.h"
#include "StructQuestManager.h"
#include "GameRule.h"
#include "NPCBase.h"

struct StructNPC : StructCreature
{
	struct NPCDeadHandler
	{
		virtual void onNPCDead( struct StructNPC * pNPC ) = 0;
	};

	struct QuestFunctor
	{
		virtual bool operator()( struct StructPlayer * pPlayer, const QuestLink & link ) = 0;
	};

	enum NPC_STATUS
	{
		STATUS_NORMAL	= 0,
		STATUS_TRACKING,
		STATUS_FIND_ATTACK_POS,
		STATUS_ATTACK,		
		STATUS_DEAD,
	};

	StructNPC( const NPCBase * npc_base );

	virtual ~StructNPC();

	virtual bool	ProcDelete()			{ delete this; return true; }

	NPC_STATUS		GetStatus() const;
	void			SetStatus( NPC_STATUS status );

	virtual const char*		GetName() const			{ return m_szName;	}

	bool			IsNPC() const			{ return true;			}

	AR_HANDLE		GetHandle() const		{ return m_hHandle;		}
	
	int				GetNPCID() const		{ return m_pBaseInfo->id;		}

	int				GetQuestTextId( QuestBase::QuestCode code, int progress );

	const char *	GetContactFunction()	{ return m_pBaseInfo->contact_script; }

	void			LinkQuest( const QuestLink & quest_link_info );
	void			ClearQuestLink()
	{
		m_vQuestLink_Start.clear();
		m_vQuestLink_Progress.clear();
		m_vQuestLink_End.clear();
	}

	virtual bool	StartAttack( AR_HANDLE target, bool bNeedFastReaction );

	void			SetAttacker( StructCreature * pAttacker );

	void			DoEachStartableQuest( struct StructPlayer * pPlayer, QuestFunctor & _fo );
	void			DoEachInProgressQuest( struct StructPlayer * pPlayer, QuestFunctor & _fo );
	void			DoEachFinishableQuest( struct StructPlayer * pPlayer, QuestFunctor & _fo );

	bool			IsAttackableNPC() const;
	bool			HasQuest() const		{ return m_bHasQuest; }
	bool			HasStartableQuest( const struct StructPlayer * pPlayer );
	bool			HasInProgressQuest( const struct StructPlayer * pPlayer );
	bool			HasFinishableQuest( const struct StructPlayer * pPlayer );

	void			SetReturnPosition( AR_UNIT x, AR_UNIT y )		{	m_RespawnX = x;	m_RespawnY = y;	}

	const NPCBase *	GetNPCBase() const							{ return m_pBaseInfo; }

	bool			IsEnemy( StructCreature* pTarget, bool bIncludeHiding = false );

	void			SetDeadHandler( NPCDeadHandler * pHandler )	{ m_pDeadHandler = pHandler; }

	bool			IsFirstAttacker() const;
	AR_UNIT			GetChaseRange() const;
	virtual bool	IsBattleMode() const { return ( GetStatus() == STATUS_TRACKING || GetStatus() == STATUS_ATTACK || GetStatus() == STATUS_FIND_ATTACK_POS ); }

	void					SetRoamer( struct StructRoamer * pRoamer )	{ m_pRoamer = pRoamer; }
	struct StructRoamer *	GetRoamer()									{ return m_pRoamer; }

	virtual void			onDead( StructCreature *pFrom, bool decreaseEXPOnDead = true );
	void		comeBackHome(bool bInvincible);

protected:
	void		findAttackablePosition( const ArPosition & myPosition, ArPosition & enemyPosition, AR_UNIT distance, AR_UNIT gap );
	void		processFirstAttack( AR_TIME t );
	AR_HANDLE	GetEnemyUID();
	void		AI_processAttack( AR_TIME t );
	ArPosition	getNonDuplicateAttackPos( StructCreature * pEnemy );
	void		processWalk( AR_TIME t );
	void		processDead( AR_TIME t );
	void		AI_processAttack( StructCreature * pEnemy, AR_TIME t );

protected:
	AR_TIME		m_nLastTrackTime;
	AR_UNIT		m_nLastEnemyDistance;
	int			m_nStatus; 
	AR_UNIT		m_RespawnX, m_RespawnY;
	bool		m_bComeBackHome;

protected:
	void onProcess( int nThreadIdx );

	virtual const CreatureStat & GetBaseStat() const;

	char			m_szName[128];
	AR_HANDLE		m_hHandle;

	bool			m_bHasQuest;

	const NPCBase *	m_pBaseInfo;

	std::vector< QuestLink >			m_vQuestLink_Start;
	std::vector< QuestLink >			m_vQuestLink_Progress;
	std::vector< QuestLink >			m_vQuestLink_End;

	std::vector< QuestBase::QuestCode > m_vQuest;				// 전체 퀘스트 리스트

	NPCDeadHandler *					m_pDeadHandler;

	struct StructRoamer *				m_pRoamer;

	// 스탯 관련 함수 및 변수들을 아래에 정리
protected:	
	// --- 이하 핸들러들 ---
	virtual void			onBeforeCalculateStat();
	virtual void			onApplyAttributeAdjustment();
};