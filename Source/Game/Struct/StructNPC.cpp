#include "StructNPC.h"
#include "StructPlayer.h"
#include "StructSummon.h"
#include "GameAllocator.h"
#include "SendMessage.h"


StructNPC::StructNPC( const NPCBase * npc_base )
{						
	m_nArObjectType = ArObject::MOVABLE_OBJECT;

	m_hHandle = AllocMiscHandle( this );

	m_bHasQuest = false;

	m_pBaseInfo = npc_base;

	std::string name = GameContent::GetString( m_pBaseInfo->text_id );
	name += " ";
	name += GameContent::GetString( m_pBaseInfo->name_text_id );

	s_strcpy( m_szName, _countof( m_szName ), name.c_str() );

	m_pRoamer = NULL;

	m_pDeadHandler = NULL;

	m_nStatus = STATUS_NORMAL;

	m_bComeBackHome = false;

	m_nLastEnemyDistance = 0;

	m_nLastTrackTime = 0;

	m_RespawnX = npc_base->x;
	m_RespawnY = npc_base->y;

	if( !IsAttackableNPC() )
		SetInvincible( true );

#ifdef _MEM_USAGE_DEBUG
	XSEH::IncreaseAllocCount( "StructNPC" );
#endif
}

AR_UNIT StructNPC::GetChaseRange() const
{
	return m_pBaseInfo->chase_range * GameRule::DEFAULT_UNIT_SIZE;
}

StructNPC::NPC_STATUS StructNPC::GetStatus() const
{
	return static_cast< NPC_STATUS >( m_nStatus );
}

void StructNPC::SetStatus( NPC_STATUS status )
{
	if( m_nStatus != status && status != STATUS_DEAD )
	{
		BroadcastStatusMessage( this );
	}

	m_nStatus = status;
}

StructNPC::~StructNPC()
{
	FreeMiscHandle( m_hHandle );

#ifdef _MEM_USAGE_DEBUG
	XSEH::DecreaseAllocCount( "StructNPC" );
#endif
}

const CreatureStat & StructNPC::GetBaseStat() const
{
	return GameContent::GetStatInfo( m_pBaseInfo->stat_id );
}

void StructNPC::LinkQuest( const QuestLink & quest_link_info )
{
	if( quest_link_info.bLF_Start )		m_vQuestLink_Start.push_back( quest_link_info );
	if( quest_link_info.bLF_Progress )	m_vQuestLink_Progress.push_back( quest_link_info );
	if( quest_link_info.bLF_End )		m_vQuestLink_End.push_back( quest_link_info );
}

void StructNPC::DoEachStartableQuest( struct StructPlayer * pPlayer,QuestFunctor & _fo )
{
	std::vector< QuestLink >::iterator it;

	for( it = m_vQuestLink_Start.begin(); it != m_vQuestLink_Start.end(); ++it )
	{
		if( pPlayer->IsStartableQuest( (*it).code ) )
		{
			_fo( pPlayer, (*it) );
		}
	}
}

void StructNPC::DoEachInProgressQuest( struct StructPlayer * pPlayer,QuestFunctor & _fo )
{
	std::vector< QuestLink >::iterator it;

	for( it = m_vQuestLink_Progress.begin(); it != m_vQuestLink_Progress.end(); ++it )
	{
		if( pPlayer->IsInProgressQuest( (*it).code ) )
		{
			_fo( pPlayer, (*it) );
		}
	}
}

void StructNPC::DoEachFinishableQuest( struct StructPlayer * pPlayer, QuestFunctor & _fo )
{
	std::vector< QuestLink >::iterator it;

	for( it = m_vQuestLink_End.begin(); it != m_vQuestLink_End.end(); ++it )
	{
		if( pPlayer->IsFinishableQuest( (*it).code ) )
		{
			_fo( pPlayer, (*it) );
		}
	}
}

bool StructNPC::HasStartableQuest( const struct StructPlayer * pPlayer )
{
	std::vector< QuestLink >::iterator it;

	bool bHasStartableRandom = false;
	bool bHasProgressRandom = false;

	for( it = m_vQuestLink_Start.begin(); it != m_vQuestLink_Start.end(); ++it )
	{
		const QuestBase & rQuestBase = StructQuest::GetQuestBase( (*it).code );

		if( rQuestBase.nType == QuestBase::QUEST_RANDOM_KILL_INDIVIDUAL || rQuestBase.nType == QuestBase::QUEST_RANDOM_COLLECT )
		{
			if( pPlayer->IsInProgressQuest( (*it).code ) || pPlayer->IsFinishableQuest( (*it).code ) )
			{
				bHasProgressRandom = true;
			}
			else if( pPlayer->IsStartableQuest( (*it).code ) ) bHasStartableRandom = true;

			continue;
		}

		if( pPlayer->IsStartableQuest( (*it).code ) ) return true;
	}

	if( bHasStartableRandom && !bHasProgressRandom )
		return true;

	return false;
}

bool StructNPC::HasInProgressQuest( const struct StructPlayer * pPlayer )
{
	std::vector< QuestLink >::iterator it;

	for( it = m_vQuestLink_Progress.begin(); it != m_vQuestLink_Progress.end(); ++it )
	{
		if( pPlayer->IsInProgressQuest( (*it).code ) ) return true;
	}

	return false;
}

bool StructNPC::HasFinishableQuest( const struct StructPlayer * pPlayer )
{
	std::vector< QuestLink >::iterator it;

	for( it = m_vQuestLink_End.begin(); it != m_vQuestLink_End.end(); ++it )
	{
		if( pPlayer->IsFinishableQuest( (*it).code ) ) return true;
	}

	return false;
}

int	StructNPC::GetQuestTextId( QuestBase::QuestCode code, int progress )
{
	std::vector< QuestLink >::iterator it;

	if( progress == ::QUEST_IS_FINISHABLE )
	{
		for( it = m_vQuestLink_End.begin(); it != m_vQuestLink_End.end(); ++it )
		{
			if( (*it).code != code ) continue;

			return (*it).nEndTextId;
		}
	}
	else if( progress == ::QUEST_IS_IN_PROGRESS )
	{
		for( it = m_vQuestLink_Progress.begin(); it != m_vQuestLink_Progress.end(); ++it )
		{
			if( (*it).code != code ) continue;

			return (*it).nInProgressTextId;
		}
	}
	else 
	{
		for( it = m_vQuestLink_Start.begin(); it != m_vQuestLink_Start.end(); ++it )
		{
			if( (*it).code != code ) continue;

			return (*it).nStartTextId;
		}
	}

	return 0;
}

bool StructNPC::IsEnemy( StructCreature* pTarget, bool bIncludeHiding )
{
	if( !StructCreature::IsEnemy( pTarget, bIncludeHiding ) )
		return false;

	if( pTarget->IsMonster() )
		return false;

	StructPlayer * pPlayer = NULL;
	if( pTarget->IsPlayer() )
	{
		pPlayer = static_cast< StructPlayer * >( pTarget );
	}
	else if( pTarget->IsSummon() )
	{
		pPlayer = static_cast< StructSummon * >( pTarget )->GetMaster();
	}

	if( pPlayer && ( pPlayer->IsPKOn() || pPlayer->IsDemoniacCharacter() || pPlayer->IsBloodyCharacter() ) )
	{
		return true;
	}

	return false;
}

bool StructNPC::IsAttackableNPC() const
{
	return m_pBaseInfo->attackable;
}

void StructNPC::onDead( StructCreature *pFrom, bool decreaseEXPOnDead )
{
	StructCreature::onDead( pFrom, decreaseEXPOnDead );

	SetStatus( STATUS_DEAD );
}
