
#include <logging/FileLog.h>
#include <mmo/ArcadiaServer.h>
#include <toolkit/XEnv.h>

#include "ErrorCode/ErrorCode.h"
#include "LogClient/LogClient.h"

#include "StructFieldProp.h"
#include "FieldPropManager.h"
#include "GameAllocator.h"
#include "StructPlayer.h"
#include "StructMonster.h"
#include "ChannelManager.h"
#include "DungeonManager.h"
#include "BattleArenaManager.h"
#include "PartyManager.h"
#include "GuildManager.h"
#include "GameMessage.h"
#include "SendMessage.h"
#include "LuaVM.h"
#include "ThreadPlayerHelper.h"


StructFieldProp::FieldPropDeleter StructFieldProp::m_deleter;

StructFieldProp* StructFieldProp::Create( StructFieldProp::FieldPropDeleteHandler * pDeleteHandler, const GameContent::FIELD_PROP_RESPAWN_INFO * pPropInfo )
{
#ifdef _DEBUG
	if( FieldPropManager::GetInstance().GetFieldPropBase( pPropInfo->nPropId ).nPropId != pPropInfo->nPropId )
	{
		return NULL;
	}
#endif

	StructFieldProp* pProp = new StructFieldProp( pDeleteHandler, pPropInfo );
	
	pProp->SetCurrentXY( pPropInfo->x, pPropInfo->y );
	pProp->SetCurrentLayer( pPropInfo->layer );

	ArcadiaServer::Instance().AddObject( pProp );

	return pProp;
}

StructFieldProp* StructFieldProp::Create( StructFieldProp::FieldPropDeleteHandler * pDeleteHandler, const GameContent::FIELD_PROP_RESPAWN_INFO * pPropInfo, const AR_UNIT & x, const AR_UNIT & y, const unsigned char nLayer )
{
#ifdef _DEBUG
	if( FieldPropManager::GetInstance().GetFieldPropBase( pPropInfo->nPropId ).nPropId != pPropInfo->nPropId )
	{
		return NULL;
	}
#endif

	StructFieldProp* pProp = new StructFieldProp( pDeleteHandler, pPropInfo );
	
	pProp->SetCurrentXY( x, y );
	pProp->SetCurrentLayer( nLayer );

	ArcadiaServer::Instance().AddObject( pProp );

	return pProp;
}

StructFieldProp* StructFieldProp::CreateWithoutAddingToWorld( FieldPropDeleteHandler * pDeleteHandler, const GameContent::FIELD_PROP_RESPAWN_INFO* pPropInfo )
{
	#ifdef _DEBUG
	if( FieldPropManager::GetInstance().GetFieldPropBase( pPropInfo->nPropId ).nPropId != pPropInfo->nPropId )
	{
		return NULL;
	}
#endif

	StructFieldProp* pProp = new StructFieldProp( pDeleteHandler, pPropInfo );
	
	pProp->SetCurrentXY( pPropInfo->x, pPropInfo->y );
	pProp->SetCurrentLayer( pPropInfo->layer );

	return pProp;
}

StructFieldProp* StructFieldProp::CreateWithoutAddingToWorld( FieldPropDeleteHandler * pDeleteHandler, const GameContent::FIELD_PROP_RESPAWN_INFO* pPropInfo, const AR_UNIT & x, const AR_UNIT & y, const unsigned char nLayer )
{
	#ifdef _DEBUG
	if( FieldPropManager::GetInstance().GetFieldPropBase( pPropInfo->nPropId ).nPropId != pPropInfo->nPropId )
	{
		return NULL;
	}
#endif

	StructFieldProp* pProp = new StructFieldProp( pDeleteHandler, pPropInfo );
	
	pProp->SetCurrentXY( x, y );
	pProp->SetCurrentLayer( nLayer );

	return pProp;
}

const bool StructFieldProp::PendFreeFieldProp( StructFieldProp * pProp )
{
	return m_deleter.Push( pProp );
}

bool StructFieldProp::ProcDelete()
{
	// FieldPropDeleter::Push 또는 StructFieldProp::PendFreeFieldProp를 사용하지 않고 다른 방법으로 FieldProp을 삭제한 경우 assert 걸림. StructFieldProp::FieldPropDeleter를 참조할 것.
	assert( !m_pDeleteHandler );

	delete this;

	return true;
}

bool StructFieldProp::IsCastable( struct StructPlayer * pPlayer ) const
{
	if( m_bIsCasting )
		return false;

	return IsUsable( pPlayer );
}

bool StructFieldProp::IsUsable( struct StructPlayer * pPlayer ) const
{
	if( pPlayer->GetLevel() < GetFieldPropBase()->nMinLevel )
		return false;

	if( pPlayer->GetLevel() > GetFieldPropBase()->nMaxLevel )
		return false;

	bool bUsable = false;

	if( GetFieldPropBase()->nLimit & FieldPropBase::LIMIT_HUNTER && pPlayer->IsHunter() ) bUsable = true;
	if( GetFieldPropBase()->nLimit & FieldPropBase::LIMIT_FIGHTER && pPlayer->IsFighter() ) bUsable = true;
	if( GetFieldPropBase()->nLimit & FieldPropBase::LIMIT_MAGICIAN && pPlayer->IsMagician() ) bUsable = true;
	if( GetFieldPropBase()->nLimit & FieldPropBase::LIMIT_SUMMONER && pPlayer->IsSummoner() ) bUsable = true;

	if( !bUsable )
		return false;

	if( pPlayer->GetRace() == JobInfo::GAIA )	if( !( GetFieldPropBase()->nLimit & FieldPropBase::LIMIT_GAIA ) ) return false;
	if( pPlayer->GetRace() == JobInfo::DEVA )	if( !( GetFieldPropBase()->nLimit & FieldPropBase::LIMIT_DEVA ) ) return false;
	if( pPlayer->GetRace() == JobInfo::ASURA )	if( !( GetFieldPropBase()->nLimit & FieldPropBase::LIMIT_ASURA ) ) return false;

	if( GetFieldPropBase()->nLimitJobId && GetFieldPropBase()->nLimitJobId != pPlayer->GetJobId() ) return false;

	for( int i = 0; i < 2; ++i )
	{
		switch( GetFieldPropBase()->nActivateId[i] )
		{
		case FieldPropBase::CHECK_TYPE_ITEM:
			{
				StructItem * pItem = pPlayer->FindItem( ItemBase::ItemCode( GetFieldPropBase()->nActivateValue[i][0] ) );

				if( !pItem || pItem->GetCount() < GetFieldPropBase()->nActivateValue[i][1] )
					return false;
			}
			break;
		
		case FieldPropBase::CHECK_TYPE_QUEST:
			{
				if( pPlayer->GetQuestProgress( GetFieldPropBase()->nActivateValue[i][0] ) != GetFieldPropBase()->nActivateValue[i][1] )
					return false;
			}
			break;

		case FieldPropBase::CHECK_TYPE_SKILL:
			{
				if( pPlayer->GetBaseSkillLevel( GetFieldPropBase()->nActivateValue[i][0] ) < GetFieldPropBase()->nActivateValue[i][1] )
					return false;
			}
			break;

		case FieldPropBase::CHECK_TYPE_WEAR:
			{
				if( GetFieldPropBase()->nActivateValue[i][1] && !pPlayer->IsWeared( ItemBase::ItemCode( GetFieldPropBase()->nActivateValue[i][0] ) ) )
					return false;
				if( !GetFieldPropBase()->nActivateValue[i][1] && pPlayer->IsWeared( ItemBase::ItemCode( GetFieldPropBase()->nActivateValue[i][0] ) ) )
					return false;
			}
			break;

		case FieldPropBase::CHECK_TYPE_SUMMON:
			{
				// not implemented
			}
			break;

		case FieldPropBase::CHECK_TYPE_PROP:
			{
				// not implemented
			}
			break;

		case FieldPropBase::CHECK_TYPE_MEMBER:
			{
				// not implemented
			}
			break;

		case FieldPropBase::CHECK_TYPE_EXIST_NEAR_MOB:
		case FieldPropBase::CHECK_TYPE_NON_EXIST_NEAR_MOB:
			{
				std::vector< AR_HANDLE > vList;
				ArcadiaServer::Instance().EnumMovableObject( pPlayer->GetCurrentPosition( GetArTime() ), pPlayer->GetLayer(), GetFieldPropBase()->nActivateValue[i][1] * GameRule::DEFAULT_UNIT_SIZE, &vList, false, true );
				
				bool bMonsterFinding = false;

				for( std::vector< AR_HANDLE >::iterator it = vList.begin(); it != vList.end(); ++it )
				{
					StructCreature::iterator itMonster = StructCreature::get( *it );

					if( !(*itMonster) ) continue;

					StructMonster * pMonster = static_cast< StructMonster* >( (*itMonster) );

					if( !pMonster->IsMonster() )
						continue;

					if( !GetFieldPropBase()->nActivateValue[i][0] )
					{
						if( GetFieldPropBase()->nActivateId[i] == FieldPropBase::CHECK_TYPE_EXIST_NEAR_MOB )
						{
							bMonsterFinding = true;
							break;
						}
						else
							return false;
					}
					else if( pMonster->GetMonsterId() == GetFieldPropBase()->nActivateValue[i][0] )
					{
						if( GetFieldPropBase()->nActivateId[i] == FieldPropBase::CHECK_TYPE_EXIST_NEAR_MOB )
						{
							bMonsterFinding = true;
							break;
						}
						else
							return false;
					}
				}

				if( GetFieldPropBase()->nActivateId[i] == FieldPropBase::CHECK_TYPE_EXIST_NEAR_MOB && !bMonsterFinding )
					return false;
			}
			break;

		case FieldPropBase::CHECK_TYPE_OWN_TACTICAL_POSITION:
			{
				ArPosition pos = pPlayer->GetPos();
				unsigned char layer = pPlayer->GetLayer();

				//int dungeon_id = ChannelManager::GetChannelId( pos.x, pos.y );
				int dungeon_id = DungeonManager::Instance().GetDungeonID( pos.x, pos.y );

				if( dungeon_id != GetFieldPropBase()->nActivateValue[i][1] )
				{
					return false;
				}

				if( layer != DungeonManager::DUNGEON_SIEGE_LAYER )
				{
					return false;
				}

				int nPartyID = pPlayer->GetPartyID();
				if( !nPartyID )
					return false;

				if( !PartyManager::GetInstance().IsAttackTeamParty( nPartyID ) )
					return false;

				int guild_id = PartyManager::GetInstance().GetAttackTeamGuildID( nPartyID );

				if( GuildManager::GetInstance().GetRaidDungeonID( guild_id ) != dungeon_id )
					return false;

				if( !DungeonManager::Instance().IsSiegeBegin( dungeon_id ) )
					return false;

				if( PartyManager::GetInstance().GetPartyType( nPartyID ) != PartyManager::TYPE_SIEGE_ATTACKTEAM )
					return false;

				if( DungeonManager::Instance().IsOwner( dungeon_id, GetFieldPropBase()->nActivateValue[i][0], guild_id ) )
					return false;
			}
			break;

		case FieldPropBase::CHECK_TYPE_NOT_OWNED_BATTLE_ARNEA_PROP:
			{
				int nPartyID = pPlayer->GetPartyID();
				if( !pPlayer->IsInBattleArena() || !nPartyID || PartyManager::GetInstance().GetPartyType( nPartyID ) != PartyManager::TYPE_BATTLE_ARENA_TEAM )
					return false;

				int nArenaID = pPlayer->GetBattleArenaID();
				if( !nArenaID )
					return false;

				unsigned char nInstanceNo = pPlayer->GetBattleArenaInstanceNo();
				if( !IsValidBattleArenaInstanceNo( nInstanceNo ) )
					return false;

				if( !BattleArenaManager::Instance().IsCastablePropForTeam( nArenaID, nInstanceNo, this, nPartyID ) )
					return false;
			}
			break;

		}
	}

	return true;
}

bool StructFieldProp::UseProp(struct StructPlayer * pPlayer )
{
	m_bIsCasting = false;

	// 월드에서 삭제 처리된 프랍이 사용되면 무조건 실패 처리
	// Virtual 함수가 아니라 Delete 된 후에 여기 들어와도 무사히 넘어가주는 경우가 의외로 종종 있을지도...
	if( !IsInWorld() )
	{
		return false;
	}

	if( GetFieldPropBase()->nUseCount )
	{
		if( InterlockedDecrement( &m_nUseCount ) < 0 )
		{
			InterlockedIncrement( &m_nUseCount );

			return false;
		}
	}

	for( int i = 0; i < 2; ++i )
	{
		int nKey = XRandom( 1, 100000000 );

		if( GetFieldPropBase()->drop_info[i].code && nKey <= ( GetFieldPropBase()->drop_info[i].ratio ) )
		{
			__int64 nItemCount = XRandom( GetFieldPropBase()->drop_info[i].min_count, GetFieldPropBase()->drop_info[i].max_count );

			__int64 nDropCount = 1;		// 곱하기 때문에 Default 1로 설정
			int nItemID = GetFieldPropBase()->drop_info[i].code;
			while( nItemID < 0 )
			{
				GameContent::SelectItemIDFromDropGroup( nItemID, nItemID, nDropCount );
			}
			nItemCount = nItemCount * nDropCount;

			StructItem* pItem = StructItem::AllocItem( 0, nItemID, nItemCount, ItemInstance::BY_FIELD_PROP, XRandom( GetFieldPropBase()->drop_info[i].min_level, GetFieldPropBase()->drop_info[i].max_level ) );

			StructItem* pNewItem = pPlayer->PushItem( pItem, pItem->GetCount() );

			// 아이템 획득 메시지를 보여주기 위해 그냥 보내주기
			if( pNewItem )
			{
				SendResult( pPlayer, TM_CS_TAKE_ITEM, RESULT_SUCCESS, pNewItem->GetHandle() );
				LOG::Log11N4S( LM_ITEM_TAKE, pPlayer->GetAccountID(), pPlayer->GetSID(), pItem->GetItemEnhance() * 100 + pItem->GetItemLevel(), pItem->GetItemCode(), pItem->GetCount(), pNewItem->GetCount(), pPlayer->GetGold().GetRawData(), pPlayer->GetGold().GetRawData(), pPlayer->GetX(), pPlayer->GetY(), 0, pPlayer->GetAccountName(), LOG::STR_NTS, pPlayer->GetName(), LOG::STR_NTS, "", 0, "PICK", LOG::STR_NTS );
			}

			if( pNewItem != pItem )
			{
				StructItem::PendFreeItem( pItem );
			}
		}
	}

	if( GetFieldPropBase()->strScript.empty() == false && _stricmp( "0", GetFieldPropBase()->strScript.c_str() ) )
	{
		LUA()->RunString( GetFieldPropBase()->strScript.c_str() );
	}

	if( m_nUseCount == 0 )
	{
		ArcadiaServer::Instance().RemoveObject( this );
		StructFieldProp::PendFreeFieldProp( this );
	}

	return true;
}

bool StructFieldProp::Cast()
{
	m_bIsCasting = true;

	return true;
}

bool StructFieldProp::CancelCast()
{
	m_bIsCasting = false;

	return true;
}

StructFieldProp::StructFieldProp( StructFieldProp::FieldPropDeleteHandler * pDeleteHandler, const GameContent::FIELD_PROP_RESPAWN_INFO* pPropInfo )
{
	m_bIsCasting = false;

	m_hHandle = AllocMiscHandle( this );

	m_nArObjectType = ArObject::STATIC_OBJECT;

	m_pFieldPropBase = &FieldPropManager::GetInstance().GetFieldPropBase( pPropInfo->nPropId );

	m_pDeleteHandler = pDeleteHandler;

	m_pPropInfo = pPropInfo;

	m_nRegenTime = GetArTime();

	m_nUseCount = m_pFieldPropBase->nUseCount;

	if( m_nUseCount == 0 )
	{
		m_nUseCount = 1;
	}

#ifdef _MEM_USAGE_DEBUG
	XSEH::IncreaseAllocCount( "StructFieldProp" );
#endif
}

AR_TIME StructFieldProp::GetCastingDelay() const
{
	return GetFieldPropBase()->nCastingTime;
}

StructFieldProp::~StructFieldProp()
{
	FreeMiscHandle( m_hHandle );

#ifdef _MEM_USAGE_DEBUG
	XSEH::DecreaseAllocCount( "StructFieldProp" );
#endif
}

StructFieldProp::FieldPropDeleter::~FieldPropDeleter()
{
	if( m_deleter.IsEnable() )
	{
		m_deleter.Disable();
		m_deleter.onProcess( 0 );
	}
}

void StructFieldProp::FieldPropDeleter::onProcess( int nThreadIdx )
{
	char buf[255];
	s_sprintf( buf, _countof( buf ), "thread.scheduler.%d.proc", nThreadIdx );
	ENV().Set( buf, "FieldPropDeleter" );

	extern __declspec( thread ) XSEH::THREAD_INFO s_ThreadInfo;
	s_sprintf( s_ThreadInfo.job_info, _countof( s_ThreadInfo.job_info ), "FieldPropDeleter(0x%08X)", (UINT_PTR)this );
	s_ThreadInfo.last_execute_time = GetArTime();
	ThreadPlayerHelper TPHelper( NULL );

	std::vector< StructFieldProp * > vDeleteList;

	// m_csDelete를 걸어놓고 순회하지 않도록 사본에 복사하는 동안만 락 걸기
	{
		THREAD_SYNCHRONIZE( m_csDelete );

		vDeleteList.swap( m_vDeleteList );

		// 삭제 대기 목록이 초기화되었으므로 Priority를 다시 IDLE로 전환
		ArcadiaServer::Instance().SetObjectPriority( this, UPDATE_PRIORITY_IDLE );
	}

	for( std::vector< StructFieldProp * >::iterator it = vDeleteList.begin() ; it != vDeleteList.end() ; ++it )
	{
		if( (*it)->m_pDeleteHandler )
		{
			(*it)->m_pDeleteHandler->onFieldPropDelete( (*it) );
			(*it)->m_pDeleteHandler = NULL;
		}

		ArcadiaServer::Instance().DeleteObject( (*it) );
	}
}

const bool StructFieldProp::FieldPropDeleter::Push( StructFieldProp * pProp )
{
	// ArcadiaServer::RemoveObject를 호출하고 Delete 요청해야 함
	assert( !pProp->IsInWorld() );

	{
		THREAD_SYNCHRONIZE( m_csDelete );

#ifdef _DEBUG
		if( std::find( m_vDeleteList.begin(), m_vDeleteList.end(), pProp ) != m_vDeleteList.end() )
		{
			assert( 0 );
			return false;
		}
#endif

		m_vDeleteList.push_back( pProp );

		if( GetFinalPriority() == ArSchedulerObject::UPDATE_PRIORITY_IDLE )
			ArcadiaServer::Instance().SetObjectPriority( this, UPDATE_PRIORITY_HIGH );
	}

	return true;
}
