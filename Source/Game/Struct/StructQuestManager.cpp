
#include <memory.h>

#include <toolkit/XConsole.h>
#include <logging/FileLog.h>

#include "StructQuestManager.h"
#include "GameContent.h"
#include "SendMessage.h"
#include "StructPlayer.h"
#include "DB_Commands.h"


StructQuestManager::StructQuestManager()
{
	m_QuestIdx = 0;

#ifdef _MEM_USAGE_DEBUG
	XSEH::IncreaseAllocCount( "StructQuestManager" );
#endif
};

StructQuestManager::~StructQuestManager()
{
	m_hsFinishedQuest.clear();
	m_hsQuestCoolTime.clear();

	for( std::vector< StructQuest * >::iterator it = m_vActiveQuest.begin(); it != m_vActiveQuest.end(); ++it )
	{
		// 메모리 풀 사용하도록 변경
		//delete (*it);
		(*it)->FreeQuest();
	}
	m_vActiveQuest.clear();

#ifdef _MEM_USAGE_DEBUG
	XSEH::DecreaseAllocCount( "StructQuestManager" );
#endif
};

const size_t StructQuestManager::DoEachActiveQuest( struct QuestFunctor & _fo )
{
	size_t nCount = 0;

	for( std::vector< StructQuest * >::iterator it = m_vActiveQuest.begin(); it != m_vActiveQuest.end(); ++it )
	{
		// 전체 퀘스트 목록 클라이언트에 송신 및 캐릭터 정보 저장에만 사용되므로 QuestInstance::FAIL 상태의 퀘스트도 포함하여 처리 함
		if( (*it)->GetProgress() != QuestInstance::FINISHED && (*it)->GetProgress() != QuestInstance::NOT_STARTED )
		{
			++nCount;
			_fo( *it );
		}
	}

	return nCount;
}

const size_t StructQuestManager::DoEachPendingQuest( struct QuestFunctor & _fo )
{
	size_t nCount = 0;

	for( std::vector< StructQuest * >::iterator it = m_vActiveQuest.begin(); it != m_vActiveQuest.end(); ++it )
	{
		if( (*it)->GetProgress() == QuestInstance::NOT_STARTED )
		{
			++nCount;
			_fo( *it );
		}
	}

	return nCount;
}

bool StructQuestManager::AddQuest( struct StructQuest* pQuest )
{
	QuestBase::QuestCode nQuestCode = pQuest->GetQuestCode();

	if( nQuestCode == 0 )
	{
		FILELOG( "Quest code is zero!\n" );
		_cprint( "Quest code is zero!\n" );
		return false;
	}

	for( std::vector< StructQuest * >::iterator it = m_vActiveQuest.begin() ; it != m_vActiveQuest.end() ; ++it )
	{
		if( nQuestCode == (*it)->GetQuestCode() )
		{
			FILELOG( "Duplicated active quest detected. PlayerUID(%d), QuestCode(%d)", static_cast< StructPlayer * >( m_pHandler )->GetSID(), nQuestCode );
			_cprint( "Duplicated active quest detected. PlayerUID(%d), QuestCode(%d)\n", static_cast< StructPlayer * >( m_pHandler )->GetSID(), nQuestCode );
			return false;
		}
	}

	if( pQuest->GetProgress() == QuestInstance::FINISHED )
	{
		if ( m_hsFinishedQuest.has( nQuestCode ) && !pQuest->IsRepeatable() )
		{
			FILELOG( "Duplicated completed quest detected. PlayerUID(%d), QuestCode(%d)", static_cast< StructPlayer * >( m_pHandler )->GetSID(), nQuestCode );
			_cprint( "Duplicated completed quest detected. PlayerUID(%d), QuestCode(%d)\n", static_cast< StructPlayer * >( m_pHandler )->GetSID(), nQuestCode );
		}
		else
			m_hsFinishedQuest.add( nQuestCode, pQuest->GetQuestID() );

		return false;
	}

	if( pQuest->IsRandomQuest() )
	{
		std::vector< RandomQuestInfo >::iterator it;

		for( it = m_vRandomQuestInfo.begin(); it != m_vRandomQuestInfo.end(); ++it )
		{
			if( (*it).code == nQuestCode )
			{
				for( int i = 0; i < QuestInstance::MAX_RANDOM_VALUE; ++i )
				{
					pQuest->SetRandomKey( i, (*it).key[i] );
					pQuest->SetRandomValue( i, (*it).value[i] );
				}
				break;
			}
		}
	}

	m_vActiveQuest.push_back( pQuest );
	
	return true;
}

const bool StructQuestManager::AddPendingQuest( const QuestBase::QuestCode code, const int nStartID )
{
	// 이미 시작 대기 중인 퀘스트인지 체크
	if( FindQuest( code ) )
		return false;

	// 시작 가능 퀘스트인지 체크
	if( !IsStartableQuest( code ) )
		return false;

	int nStatus[QuestInstance::MAX_STATUS];
	memset( &nStatus, 0, sizeof( nStatus ) );

	// 시작 대기 퀘스트 정보는 제한 시간 정보를 사용하지 않고 StartQuest에서 세팅함
	StructQuest * pQuest = StructQuest::AllocQuest( m_pHandler, allocQuestID(), code, nStatus, 0, QuestInstance::NOT_STARTED, nStartID );
	m_vActiveQuest.push_back( pQuest );

	// 랜덤 퀘스트일 경우 미리 랜덤 정보를 결정해 둬야 한다면 여기서 결정하면 되지만 현재는 미리 세팅하지 않고 StartQuest에서 세팅함

	return true;
}

bool StructQuestManager::StartQuest( QuestBase::QuestCode code, int nStartID )
{
	if( !code || !nStartID )
	{
		assert( 0 );
		return false;
	}

	// 시작 대기 상태가 아닌 퀘스트 정보가 있는지 외부에서 체크한 것과 별도로 다시 한 번 확인
	StructQuest * pQuest = FindQuest( code );
	if( pQuest && pQuest->GetProgress() != QuestInstance::NOT_STARTED )
	{
		return false;
	}

	AR_TIME nTimeLimit = ( StructQuest::GetQuestBase( code ).eTimeLimitType != QuestBase::TIME_LIMIT_TYPE_PERMANENT ) ? GetArTime() + StructQuest::GetQuestBase( code ).nTimeLimit * 100 : 0;

	// 시작 대기 상태의 퀘스트일 경우 기존에 추가되어 있는 퀘스트 정보 사용
	if( pQuest )
		pQuest->SetTimeLimit( nTimeLimit );
	else
	{
		int nStatus[QuestInstance::MAX_STATUS];
		memset( &nStatus, 0, sizeof( nStatus ) );

		pQuest = StructQuest::AllocQuest( m_pHandler, allocQuestID(), code, nStatus, nTimeLimit, QuestInstance::IN_PROGRESS, nStartID );
	}

	if( pQuest->IsRandomQuest() )
	{
		RandomQuestInfo * pQuestInfo = NULL;
		bool bNeedToUpdate = true;

		std::vector< RandomQuestInfo >::iterator it;

		for( it = m_vRandomQuestInfo.begin(); it != m_vRandomQuestInfo.end(); ++it )
		{
			if( (*it).code == code )
			{
				pQuestInfo = &(*it);
				bNeedToUpdate = !pQuestInfo->is_dropped;
				break;
			}
		}

		if( !pQuestInfo )
		{
			m_vRandomQuestInfo.push_back( RandomQuestInfo( code ) );
			pQuestInfo = &( m_vRandomQuestInfo.back() );
		}

		if( bNeedToUpdate )
		{
			int group_id = 0;
			int prev_group_id = 0;

			std::vector< GameContent::RANDOM_POOL_INFO > vRandomPoolInfo;

			for( int i = 0; i < QuestInstance::MAX_RANDOM_VALUE; ++i )
				pQuestInfo->key[i] = pQuestInfo->value[i] = 0;

			for( int i = 0; i < QuestInstance::MAX_RANDOM_VALUE; ++i )
			{
				group_id = pQuest->GetValue( i * 4 );

				if( !group_id )
					break;

				if( group_id != prev_group_id && !GameContent::GetRandomPoolInfo( group_id, vRandomPoolInfo, pQuest->GetQuestBase().nLimitLevel ) )
					assert( 0 );

				if( !vRandomPoolInfo.size() )
				{
					pQuest->FreeQuest();
					return false;
				}

				prev_group_id = group_id;

				size_t idx = 0;

				idx = XRandom() % vRandomPoolInfo.size();

				pQuestInfo->key[i] = vRandomPoolInfo[idx].quest_target_id;
				pQuestInfo->value[i] = XRandom( pQuest->GetValue( i * 4 + 1 ), pQuest->GetValue( i * 4 + 2 ) );

				vRandomPoolInfo.erase( vRandomPoolInfo.begin() + idx );
			}
		}

		pQuestInfo->is_dropped = false;

		for( int i = 0; i < QuestInstance::MAX_RANDOM_VALUE; ++i )
		{
			pQuest->SetRandomKey( i, pQuestInfo->key[i] );
			pQuest->SetRandomValue( i, pQuestInfo->value[i] );
		}
	}

	// 수락/포기 쿨타임이 있는 퀘스트에 한해서만 쿨타임 설정
	if( pQuest->GetQuestBase().nDropCoolTime )
	{
		time_t tCoolTime = time( NULL ) + pQuest->GetQuestBase().nDropCoolTime;

		AddQuestCoolTime( pQuest->GetQuestCode(), tCoolTime, QuestInstance::IN_PROGRESS );
	}

	// 시작 대기 상태의 퀘스트였을 경우 진행 상태만 변경, 아니라면 새로 목록에 추가
	if( pQuest->GetProgress() == QuestInstance::NOT_STARTED )
		pQuest->SetProgress( QuestInstance::IN_PROGRESS );
	else
	{
		m_vActiveQuest.push_back( pQuest );
	}

	return true;
}

bool StructQuestManager::EndQuest( struct StructQuest* pQuest )
{
	pQuest->SetProgress( QuestInstance::FINISHED );

	if( !m_hsFinishedQuest.has( pQuest->GetQuestCode() ) )
	{
		m_hsFinishedQuest.add( pQuest->GetQuestCode(), pQuest->GetQuestID() );
	}

	// 반복퀘가 아니더라도 퀘스트를 다시 받을 방법이 존재하므로, 쿨타임을 사용하게 되어있다면 의도된 것으로 판단하여 쿨타임 설정을 해준다.
	if( pQuest->GetQuestBase().nCoolTime )
	{
		time_t tCoolTime = time( NULL ) + pQuest->GetQuestBase().nCoolTime;

		// 완료 후 재수락 쿨타임(단위: 초, 0 : 사용 안 함, 음수 : 일일 단위 초기화)
		if( pQuest->GetQuestBase().nCoolTime < 0 )
		{
			tCoolTime = static_cast< StructPlayer * >( m_pHandler )->GetNextHuntaholicEnterableCountRefill();			
		}

		AddQuestCoolTime( pQuest->GetQuestCode(), tCoolTime, QuestInstance::FINISHED );
	}

	return true;
}

const bool StructQuestManager::ProcExpiredQuest( const AR_TIME t )
{
	bool bExpired = false;

	for( std::vector< StructQuest * >::iterator it = m_vActiveQuest.begin() ; it != m_vActiveQuest.end() ; ++it )
	{
		// 진행 중인 퀘스트가 아니면 패스
		if( (*it)->GetProgress() != QuestInstance::IN_PROGRESS )
			continue;

		// 시간 무제한 퀘스트 제외
		if( (*it)->GetTimeLimitType() == QuestBase::TIME_LIMIT_TYPE_PERMANENT )
			continue;

		// 시간 경과 타입에 관계 없이 GetTimeLimit으로 나온 값이 제한 시간
		if( (*it)->GetTimeLimit() >= t )
			continue;

		// 만료된 퀘스트 처리
		bExpired = true;
		(*it)->SetProgress( QuestInstance::FAIL );
	}

	if( bExpired )
		SendNPCStatusInVisibleRange( static_cast< StructPlayer * >( m_pHandler ) );

	return bExpired;
}


struct StructQuest*	StructQuestManager::FindQuest( QuestBase::QuestCode code ) const
{
	std::vector< StructQuest * >::const_iterator it;

	for( it = m_vActiveQuest.begin(); it != m_vActiveQuest.end(); ++it )
	{
		if( (*it)->GetQuestCode() == code && (*it)->GetProgress() != QuestInstance::FINISHED )
		{
			return (*it);
		}
	}

	return NULL;
}

const bool StructQuestManager::IsPendingQuest( const QuestBase::QuestCode code ) const
{
	StructQuest * pQuest = FindQuest( code );

	return ( pQuest && pQuest->GetProgress() == QuestInstance::NOT_STARTED );
}

bool StructQuestManager::IsFinishedQuest( QuestBase::QuestCode code ) const
{
	return m_hsFinishedQuest.has( code );
}

QuestInstance::QuestID StructQuestManager::GetFinishedQuestID( QuestBase::QuestCode code ) const
{
	QuestInstance::QuestID id = 0;

	m_hsFinishedQuest.lookup( code, id );

	return id;
}

const bool StructQuestManager::ResetFinishedQuest( QuestBase::QuestCode code )
{
	return m_hsFinishedQuest.erase( code );
}

static bool checkQuestTypeFlag( QuestBase::QuestType type, int flag )
{
	if( flag == QuestBase::TYPE_FLAG_ALL ) return true;

	{
		switch( type )
		{
			case QuestBase::QUEST_MISC:
			case QuestBase::QUEST_EXTERNAL_CONTROL:				return ( QuestBase::TYPE_FLAG_MISC							& flag );
			case QuestBase::QUEST_KILL_TOTAL:					return ( QuestBase::TYPE_FLAG_KILL_TOTAL					& flag );
			case QuestBase::QUEST_KILL_INDIVIDUAL:				return ( QuestBase::TYPE_FLAG_KILL_INDIVIDUAL				& flag );
			case QuestBase::QUEST_COLLECT:						return ( QuestBase::TYPE_FLAG_COLLECT						& flag );
			case QuestBase::QUEST_LEARN_SKILL:					return ( QuestBase::TYPE_FLAG_LEARN_SKILL					& flag );
			case QuestBase::QUEST_HUNT_ITEM:					return ( QuestBase::TYPE_FLAG_HUNT_ITEM						& flag );
			case QuestBase::QUEST_HUNT_ITEM_FROM_ANY_MONSTERS:	return ( QuestBase::TYPE_FLAG_HUNT_ITEM_FROM_ANY_MONSTSERS	& flag );
			case QuestBase::QUEST_HUNT_ITEM_DROP_PENALTY:		return ( QuestBase::TYPE_FLAG_HUNT_ITEM_FROM_ANY_MONSTSERS	& flag );
			case QuestBase::QUEST_KILL_PLAYER:					return ( QuestBase::TYPE_FLAG_KILL_PLAYER					& flag );
			case QuestBase::QUEST_UPGRADE_ITEM:					return ( QuestBase::TYPE_FLAG_UPGRADE_ITEM					& flag );
			case QuestBase::QUEST_CONTACT:						return ( QuestBase::TYPE_FLAG_CONTACT						& flag );
			case QuestBase::QUEST_JOB_LEVEL:					return ( QuestBase::TYPE_FLAG_JOB_LEVEL						& flag );
			case QuestBase::QUEST_PARAMETER:					return ( QuestBase::TYPE_FLAG_PARAMETER						& flag );
			case QuestBase::QUEST_RANDOM_COLLECT:				return ( QuestBase::TYPE_FLAG_RANDOM_COLLECT				& flag );
			case QuestBase::QUEST_RANDOM_KILL_INDIVIDUAL:		return ( QuestBase::TYPE_FLAG_RANDOM_KILL_INDIVIDUAL		& flag );
				
			// by Testors
			//default:								assert( 0 );											return false;
		}
	}

	return false;
}

bool StructQuestManager::IsTakeableQuestItem( QuestBase::QuestCode code ) const
{
	std::vector< StructQuest * >::const_iterator it;

	for( it = m_vActiveQuest.begin(); it != m_vActiveQuest.end(); ++it )
	{
		if( (*it)->GetProgress() == QuestInstance::FINISHED || (*it)->GetProgress() == QuestInstance::FAIL )	continue;

		if( (*it)->GetQuestType() != QuestBase::QUEST_COLLECT &&
			(*it)->GetQuestType() != QuestBase::QUEST_HUNT_ITEM &&
			(*it)->GetQuestType() != QuestBase::QUEST_HUNT_ITEM_FROM_ANY_MONSTERS &&
			(*it)->GetQuestType() != QuestBase::QUEST_HUNT_ITEM_DROP_PENALTY &&
			(*it)->GetQuestType() != QuestBase::QUEST_RANDOM_COLLECT ) continue;
		
		if( (*it)->IsFinishable() ) continue;

		if( (*it)->IsRandomQuest() )
		{
			if( (*it)->GetRandomKey( 0 ) == code ||
				(*it)->GetRandomKey( 1 ) == code ||
				(*it)->GetRandomKey( 2 ) == code ) return true;
		}
		else if( (*it)->GetQuestType() == QuestBase::QUEST_COLLECT )
		{
			if( (*it)->GetValue( 0 ) == code ||
				(*it)->GetValue( 2 ) == code ||
				(*it)->GetValue( 4 ) == code ||
				(*it)->GetValue( 6 ) == code ||
				(*it)->GetValue( 8 ) == code ||
				(*it)->GetValue( 10 ) == code ) return true;
		}
		else
		{
			if( (*it)->GetValue( 0 ) == code ||
				(*it)->GetValue( 2 ) == code ||
				(*it)->GetValue( 4 ) == code ) return true;
		}
	}

	return false;
}

void StructQuestManager::GetRelatedQuest( std::vector< StructQuest* > & vQuestList, int flag )
{
	std::vector< StructQuest * >::iterator it;

	for( it = m_vActiveQuest.begin(); it != m_vActiveQuest.end(); ++it )
	{
		if( (*it)->GetProgress() == QuestInstance::FINISHED || (*it)->GetProgress() == QuestInstance::FAIL )	continue;

		// 조건검사
		if( !checkQuestTypeFlag( (*it)->GetQuestType(), flag ) ) continue;
	
		vQuestList.push_back( *it );
	}
}

void StructQuestManager::GetRelatedQuestByItem( ItemBase::ItemCode code, std::vector< StructQuest* > & vQuestList, int flag )
{
	std::vector< StructQuest * >::iterator it;

	for( it = m_vActiveQuest.begin(); it != m_vActiveQuest.end(); ++it )
	{
		if( (*it)->GetProgress() == QuestInstance::FINISHED || (*it)->GetProgress() == QuestInstance::FAIL )	continue;

		// 조건검사
		if( !checkQuestTypeFlag( (*it)->GetQuestType(), flag ) ) continue;
	
		// 사냥 수집퀘스트
		if( (*it)->GetQuestType() == QuestBase::QUEST_HUNT_ITEM ||
			(*it)->GetQuestType() == QuestBase::QUEST_HUNT_ITEM_FROM_ANY_MONSTERS ||
			(*it)->GetQuestType() == QuestBase::QUEST_HUNT_ITEM_DROP_PENALTY )
		{
			if( (*it)->GetValue( 0 ) == code || (*it)->GetValue( 2 ) == code || (*it)->GetValue( 4 ) == code ) vQuestList.push_back( *it );
			continue;
		}

		// 수집퀘스트
		if( (*it)->GetQuestType() == QuestBase::QUEST_COLLECT )
		{
			if( (*it)->GetValue( 0 ) == code || (*it)->GetValue( 2 ) == code || (*it)->GetValue( 4 ) == code ||
				(*it)->GetValue( 6 ) == code || (*it)->GetValue( 8 ) == code || (*it)->GetValue( 10 ) == code ) vQuestList.push_back( *it );
			continue;
		}

		// 랜덤 수집 퀘스트
		if( (*it)->GetQuestType() == QuestBase::QUEST_RANDOM_COLLECT )
		{
			if( (*it)->GetRandomKey( 0 ) == code || (*it)->GetRandomKey( 1 ) == code || (*it)->GetRandomKey( 2 ) == code ) vQuestList.push_back( *it );
			continue;
		}
	}
}

void StructQuestManager::UpdateQuestStatusByItemCount( ItemBase::ItemCode code, const __int64 & count )
{
	std::vector< StructQuest* > vQuestList;

	GetRelatedQuestByItem( code, vQuestList, QuestBase::TYPE_FLAG_COLLECT | QuestBase::TYPE_FLAG_HUNT_ITEM | QuestBase::TYPE_FLAG_HUNT_ITEM_FROM_ANY_MONSTSERS | QuestBase::TYPE_FLAG_RANDOM_COLLECT );

	for( std::vector< StructQuest* >::iterator it = vQuestList.begin(); it != vQuestList.end(); ++it )
	{
		StructQuest * pQuest = (*it);

		bool bFinishable = pQuest->IsFinishable();

		for( int i = 0; i < QuestInstance::MAX_STATUS; ++i )
		{
			if( pQuest->IsRandomQuest() )
			{
				if( pQuest->GetRandomKey( i ) == code )
				{	
					pQuest->UpdateStatus( i, std::min( pQuest->GetRandomValue( i ), static_cast< int >( count ) ) );
					break;
				}
			}
			else
			{
				if( pQuest->GetValue( i * 2 ) == code )
				{	
					pQuest->UpdateStatus( i, std::min( (int) pQuest->GetValue( i * 2 + 1), static_cast< int >( count ) ) );
					break;
				}
			}
		}

		if( bFinishable != pQuest->IsFinishable() )
		{
			// 완료 가능하던 퀘스트가 완료 불가능하게 변경되었으면 실패 처리
			if( bFinishable )
			{
				pQuest->SetProgress( QuestInstance::FAIL );
			}
			// 완료 불가능하던 퀘스트가 완료 가능하게 되었으면 종료 가능 상태로 변경(시간제 퀘스트 및 불필요한 체크 중지)
			else
			{
				pQuest->SetProgress( QuestInstance::FINISHABLE );
			}
		}
	}
}

void StructQuestManager::UpdateQuestStatusByItemEnhance( StructInventory* inventory, const ItemBase::ItemCode code )
{
	// 어차피 퀘스트 전체를 순회할텐데 굳이 GetRelatedQuest이나 flag 등의 불필요한 추상화를 도입한 의도를 모르겠음
	std::vector< StructItem* > itemList;

	for( std::vector< StructQuest* >::iterator questIt = m_vActiveQuest.begin(); questIt != m_vActiveQuest.end(); ++questIt )
	{
		StructQuest* quest = *questIt;

		if( quest->GetQuestType() != QuestBase::QUEST_ENHANCE_ITEM ||
			quest->GetProgress() == QuestInstance::FINISHED ||
			quest->GetProgress() == QuestInstance::FAIL )
		{
			continue;
		}

		bool bFinishable = quest->IsFinishable();

		for( int i = 0 ; i < 3 ; i++ )
		{
			ItemBase::ItemCode targetItemCode = quest->GetValue( i*2 );

			// 인자로 주어진 code가 0인 경우는 예외 없이 체크
			if( targetItemCode != 0 && ( targetItemCode == code || code == 0 ) )
			{
				itemList.clear();
				inventory->Find( targetItemCode, itemList );

				int enhance = 0;

				for( std::vector< StructItem* >::iterator itemIt = itemList.begin(); itemIt != itemList.end(); itemIt++ )
				{
					StructItem* item = *itemIt;
					enhance = std::max( item->GetItemEnhance(), enhance );
				}

				quest->UpdateStatus( i, std::min( enhance, quest->GetValue( i*2 + 1 ) ) );
			}			
		}

		if( bFinishable != quest->IsFinishable() )
		{
			// 완료 가능하던 퀘스트가 완료 불가능하게 변경되었으면 실패 처리
			if( bFinishable )
			{
				quest->SetProgress( QuestInstance::FAIL );
			}
			// 완료 불가능하던 퀘스트가 완료 가능하게 되었으면 종료 가능 상태로 변경(시간제 퀘스트 및 불필요한 체크 중지)
			else
			{
				quest->SetProgress( QuestInstance::FINISHABLE );
			}
		}
	}
}

void StructQuestManager::UpdateQuestStatusByParameter( int parameter_id, int value )
{
	std::vector< StructQuest* > vQuestList;

	GetRelatedQuest( vQuestList, QuestBase::TYPE_FLAG_PARAMETER );

	for( std::vector< StructQuest * >::iterator it = vQuestList.begin(); it != vQuestList.end(); ++it )
	{
		bool bFinishable = (*it)->IsFinishable();

		for( int nIndex = 0 ; nIndex <= 6 ; nIndex += 3 )
		{
			if( (*it)->GetValue( nIndex ) == parameter_id )
			{
				(*it)->UpdateStatus( nIndex / 3, value );
			}
		}

		if( bFinishable != (*it)->IsFinishable() )
		{
			// 완료 가능하던 퀘스트가 완료 불가능하게 변경되었으면 실패 처리
			if( bFinishable )
			{
				(*it)->SetProgress( QuestInstance::FAIL );
			}
			// 완료 불가능하던 퀘스트가 완료 가능하게 되었으면 종료 가능 상태로 변경(시간제 퀘스트 및 불필요한 체크 중지)
			else
			{
				(*it)->SetProgress( QuestInstance::FINISHABLE );
			}
		}

		continue;
	}	
}

void StructQuestManager::UpdateQuestStatusByJobLevel( int nJobDepth, int nJobLevel )
{
	std::vector< StructQuest* > vQuestList;

	GetRelatedQuest( vQuestList, QuestBase::TYPE_FLAG_JOB_LEVEL );

	for( std::vector< StructQuest * >::iterator it = vQuestList.begin(); it != vQuestList.end(); ++it )
	{
		bool bFinishable = (*it)->IsFinishable();

		(*it)->UpdateStatus( 0, std::min( (int) (*it)->GetValue( 0 ), nJobDepth ) );
		(*it)->UpdateStatus( 1, std::min( (int) (*it)->GetValue( 1 ), nJobLevel ) );

		if( bFinishable != (*it)->IsFinishable() )
		{
			// 완료 가능하던 퀘스트가 완료 불가능하게 변경되었으면 실패 처리
			if( bFinishable )
			{
				(*it)->SetProgress( QuestInstance::FAIL );
			}
			// 완료 불가능하던 퀘스트가 완료 가능하게 되었으면 종료 가능 상태로 변경(시간제 퀘스트 및 불필요한 체크 중지)
			else
			{
				(*it)->SetProgress( QuestInstance::FINISHABLE );
			}
		}

		continue;
	}	
}

void StructQuestManager::UpdateQuestStatusBySkillLevel( int nSkillId, int nSkillLevel )
{
	std::vector< StructQuest* > vQuestList;

	GetRelatedQuest( vQuestList, QuestBase::TYPE_FLAG_LEARN_SKILL );

	for( std::vector< StructQuest * >::iterator it = vQuestList.begin(); it != vQuestList.end(); ++it )
	{
		bool bFinishable = (*it)->IsFinishable();

		for( int nIndex = 0 ; nIndex < QuestBase::MAX_VALUE_NUMBER ; nIndex += 2 )
		{
			if( (*it)->GetValue( nIndex ) == nSkillId )
			{
				(*it)->UpdateStatus( nIndex / 2, std::min( (int) (*it)->GetValue( nIndex + 1 ), nSkillLevel ) );
				break;
			}
		}

		if( bFinishable != (*it)->IsFinishable() )
		{
			// 완료 가능하던 퀘스트가 완료 불가능하게 변경되었으면 실패 처리
			if( bFinishable )
			{
				(*it)->SetProgress( QuestInstance::FAIL );
			}
			// 완료 불가능하던 퀘스트가 완료 가능하게 되었으면 종료 가능 상태로 변경(시간제 퀘스트 및 불필요한 체크 중지)
			else
			{
				(*it)->SetProgress( QuestInstance::FINISHABLE );
			}
		}

		continue;
	}
}

void StructQuestManager::UpdateQuestStatusByPlayerKill( const struct StructPlayer * pKilledPlayer )
{
	std::vector< StructQuest * > vQuestList;
	GetRelatedQuest( vQuestList, QuestBase::TYPE_FLAG_KILL_PLAYER );

	for( std::vector< StructQuest * >::iterator it = vQuestList.begin() ; it != vQuestList.end() ; ++it )
	{
		// 이미 조건 완료면 KIN
		if( (*it)->IsFinishable() )
			continue;

		if( (*it)->GetValue( 0 ) == pKilledPlayer->GetRace() && (*it)->GetStatus( 0 ) < (*it)->GetValue( 1 ) )
			(*it)->IncStatus( 0 );
		if( (*it)->GetValue( 2 ) == pKilledPlayer->GetJobId() && (*it)->GetStatus( 1 ) < (*it)->GetValue( 3 ) )
			(*it)->IncStatus( 1 );

		// 완료 불가능하던 퀘스트가 완료 가능하게 되었으면 종료 가능 상태로 변경(시간제 퀘스트 및 불필요한 체크 중지)
		if( (*it)->IsFinishable() )
		{
			(*it)->SetProgress( QuestInstance::FINISHABLE );
		}
	}
}

void StructQuestManager::UpdateQuestStatusByMonsterKill( int nMonsterId )
{
	std::vector< StructQuest* > vQuestList;
	GetRelatedQuest( vQuestList, QuestBase::TYPE_FLAG_KILL );

	for( std::vector< StructQuest * >::iterator it = vQuestList.begin(); it != vQuestList.end(); ++it )
	{
		// 이미 조건 완료면 KIN
		if( (*it)->IsFinishable() ) continue;

		switch( (*it)->GetQuestType() )
		{
		case QuestBase::QUEST_KILL_TOTAL:
			if( GameContent::IsInRandomPoolMonster( (*it)->GetValue( 0 ), nMonsterId ) ||
				GameContent::IsInRandomPoolMonster( (*it)->GetValue( 2 ), nMonsterId ) ||
				GameContent::IsInRandomPoolMonster( (*it)->GetValue( 4 ), nMonsterId ) )
			{
				if( (*it)->GetStatus( 0 ) < (*it)->GetValue( 1 ) )
					(*it)->IncStatus( 0 );
			}
			break;

		case QuestBase::QUEST_KILL_INDIVIDUAL:
			if( GameContent::IsInRandomPoolMonster( (*it)->GetValue( 0 ), nMonsterId ) && (*it)->GetStatus( 0 ) < (*it)->GetValue( 1 ) )
				(*it)->IncStatus( 0 );
			else if( GameContent::IsInRandomPoolMonster( (*it)->GetValue( 2 ), nMonsterId ) && (*it)->GetStatus( 1 ) < (*it)->GetValue( 3 ) )
				(*it)->IncStatus( 1 );
			else if( GameContent::IsInRandomPoolMonster( (*it)->GetValue( 4 ), nMonsterId ) && (*it)->GetStatus( 2 ) < (*it)->GetValue( 5 ) )
				(*it)->IncStatus( 2 );
			break;

		case QuestBase::QUEST_RANDOM_KILL_INDIVIDUAL:
			if( (*it)->GetRandomKey( 0 ) == nMonsterId && (*it)->GetStatus( 0 ) < (*it)->GetRandomValue( 0 ) )
				(*it)->IncStatus( 0 );
			else if( (*it)->GetRandomKey( 1 ) == nMonsterId && (*it)->GetStatus( 1 ) < (*it)->GetRandomValue( 1 ) )
				(*it)->IncStatus( 1 );
			else if( (*it)->GetRandomKey( 2 ) == nMonsterId && (*it)->GetStatus( 2 ) < (*it)->GetRandomValue( 2 ) )
				(*it)->IncStatus( 2 );
			break;
		}

		// 완료 불가능하던 퀘스트가 완료 가능하게 되었으면 종료 가능 상태로 변경(시간제 퀘스트 및 불필요한 체크 중지)
		if( (*it)->IsFinishable() )
		{
			(*it)->SetProgress( QuestInstance::FINISHABLE );
		}
	}
}

void StructQuestManager::GetRelatedQuestByMonster( int nMonsterId, std::vector< StructQuest* > & vQuestList, int flag )
{
	std::vector< StructQuest * >::iterator it;

	for( it = m_vActiveQuest.begin(); it != m_vActiveQuest.end(); ++it )
	{
		if( (*it)->GetProgress() == QuestInstance::FINISHED || (*it)->GetProgress() == QuestInstance::FAIL )	continue;

		// 조건검사
		if( !checkQuestTypeFlag( (*it)->GetQuestType(), flag ) ) continue;

		// 학살 퀘스트
		if( (*it)->GetQuestType() == QuestBase::QUEST_KILL_TOTAL || (*it)->GetQuestType() == QuestBase::QUEST_KILL_INDIVIDUAL )
		{
			if( GameContent::IsInRandomPoolMonster( (*it)->GetValue( 0 ), nMonsterId ) ||
				GameContent::IsInRandomPoolMonster( (*it)->GetValue( 2 ), nMonsterId ) ||
				GameContent::IsInRandomPoolMonster( (*it)->GetValue( 4 ), nMonsterId ) )
			{
				vQuestList.push_back( *it );
			}

			continue;
		}

		// 사냥 수집 퀘스트
		if( (*it)->GetQuestType() == QuestBase::QUEST_HUNT_ITEM )
		{
			if( GameContent::IsInRandomPoolMonster( (*it)->GetValue( 6 ), nMonsterId ) ||
				GameContent::IsInRandomPoolMonster( (*it)->GetValue( 7 ), nMonsterId ) ||
				GameContent::IsInRandomPoolMonster( (*it)->GetValue( 8 ), nMonsterId ) ||
				GameContent::IsInRandomPoolMonster( (*it)->GetValue( 9 ), nMonsterId ) ||
				GameContent::IsInRandomPoolMonster( (*it)->GetValue( 10 ), nMonsterId ) ||
				GameContent::IsInRandomPoolMonster( (*it)->GetValue( 11 ), nMonsterId ) )
			{
				vQuestList.push_back( *it );
			}

			continue;
		}
		
		// 사냥 수집 퀘스트(어떠한 몬스터든)
		if( (*it)->GetQuestType() == QuestBase::QUEST_HUNT_ITEM_FROM_ANY_MONSTERS ||
			(*it)->GetQuestType() == QuestBase::QUEST_HUNT_ITEM_DROP_PENALTY )
		{
			vQuestList.push_back( *it );

			continue;
		}
	}
}

void StructQuestManager::SetMaxQuestID( QuestInstance::QuestID id )
{
	if( m_QuestIdx )
	{
		throw XException( "SetMaxQuestID가 한 매니저에 대해 두번 불렸음." );
	}

	m_QuestIdx = id;
}

QuestInstance::QuestID	StructQuestManager::allocQuestID()
{
	return InterlockedIncrement( &m_QuestIdx );
}

bool StructQuestManager::IsStartableQuest( const QuestBase::QuestCode code ) const
{
	// 시작 대기 상태가 아닌 퀘스트가 이미 있다면 시작 불가
	StructQuest * pQuest = FindQuest( code );
	if( pQuest && pQuest->GetProgress() != QuestInstance::NOT_STARTED )
	{
		return false;
	}

	const QuestBase & rQuestBase = StructQuest::GetQuestBase( code );

	// 이미 끝냈고 반복가능퀘스트가 아니면 KIN
	if( !rQuestBase.bIsRepeatable && IsFinishedQuest( code ) )
	{
		return false;
	}

	// 선행조건
	bool bRtn = true;
	if( rQuestBase.bForeCheckType ) bRtn = false;
	for( int i = 0; i < QuestBase::MAX_FOREQUEST; ++i )
	{
		// or 타입
		if( rQuestBase.bForeCheckType )
		{
			if( rQuestBase.nForeQuest[i] && IsFinishedQuest( rQuestBase.nForeQuest[i] ) ) bRtn = true;
			
		}
		// and 타입
		else
		{
			if( rQuestBase.nForeQuest[i] && !IsFinishedQuest( rQuestBase.nForeQuest[i] ) ) return false;
		}
	}

	return bRtn;
}

void StructQuestManager::PopFromActiveQuest( struct StructQuest * pQuest )
{
	std::vector< StructQuest * >::iterator it = std::find( m_vActiveQuest.begin(), m_vActiveQuest.end(), pQuest );

	if( it != m_vActiveQuest.end() )
	{
		m_vActiveQuest.erase( it );

		pQuest->FreeQuest();
	}	
}

void StructQuestManager::SetDropFlagToRandomQuestInfo( int code )
{
	std::vector< RandomQuestInfo >::iterator it;

	for( it = m_vRandomQuestInfo.begin(); it != m_vRandomQuestInfo.end(); ++it )
	{
		if( (*it).code == code )
		{
			(*it).is_dropped = true;
			return;
		}
	}
}

bool StructQuestManager::HasRandomQuestInfo( int code ) const
{
	std::vector< RandomQuestInfo >::const_iterator it;

	for( it = m_vRandomQuestInfo.begin(); it != m_vRandomQuestInfo.end(); ++it )
	{
		if( (*it).code == code )
			return true;
	}

	return false;
}

bool StructQuestManager::NeedToUpdateRandomQuestInfo( int code ) const
{
	std::vector< RandomQuestInfo >::const_iterator it;

	for( it = m_vRandomQuestInfo.begin(); it != m_vRandomQuestInfo.end(); ++it )
	{
		if( (*it).code == code )
		{
			if( (*it).is_dropped )
				return false;
			else
				return true;
		}
	}

	return false;
}

void StructQuestManager::AddRandomQuestInfo( int code, int key[], int value[], bool is_dropped )
{
	m_vRandomQuestInfo.push_back( RandomQuestInfo( code ) );

	RandomQuestInfo * pQuestInfo = &(m_vRandomQuestInfo.back());

	for( int i = 0; i < QuestInstance::MAX_RANDOM_VALUE; ++i )
	{
		pQuestInfo->key[i] = key[i];
		pQuestInfo->value[i] = value[i];
	}

	pQuestInfo->is_dropped = is_dropped;
}

void StructQuestManager::ReadQuestCoolTime( int code, AR_TIME cool_time, QuestInstance::QUEST_PROGRESS progress )
{
	m_hsQuestCoolTime.add( code, new QuestCoolTimeInfo( cool_time, progress ) );
}

void StructQuestManager::AddQuestCoolTime( int code, AR_TIME cool_time, QuestInstance::QUEST_PROGRESS progress )
{
	QuestCoolTimeInfo *pQuestCoolTime = NULL;

	StructPlayer *pPlayer = static_cast< StructPlayer * >( m_pHandler );

	// 존재한다면 업데이트
	if( m_hsQuestCoolTime.lookup( code, pQuestCoolTime ) )
	{
		// 가장 긴 쿨타임 하나만 유지하며 progress를 이용하여 완료 쿨타임과 재수락 쿨타임의 툴팁을 다르게 표현할 수 있도록 기록
		if( pQuestCoolTime->cool_time < cool_time )
		{
			pQuestCoolTime->cool_time = cool_time;
			pQuestCoolTime->progress = progress;	

			pPlayer->DBQuery( new DB_UpdateQuestCoolTime( pPlayer, pPlayer->GetSID(), code, cool_time, progress ) );
		}
	}
	else
	{
		pQuestCoolTime = new QuestCoolTimeInfo( cool_time, progress );
		m_hsQuestCoolTime.add( code, pQuestCoolTime );

		pPlayer->DBQuery( new DB_InsertQuestCoolTime( pPlayer, pPlayer->GetSID(), code, cool_time, progress ) );
	}
}

void StructQuestManager::RemoveQuestCoolTime( int code )
{
	QuestCoolTimeInfo *pQuestCoolTime = NULL;

	StructPlayer *pPlayer = static_cast< StructPlayer * >( m_pHandler );

	if( m_hsQuestCoolTime.lookup( code, pQuestCoolTime ) )
	{
		delete pQuestCoolTime;
		m_hsQuestCoolTime.erase( code );

		pPlayer->DBQuery( new DB_DeleteQuestCoolTime( pPlayer, pPlayer->GetSID(), code ) );
	}
}

const time_t StructQuestManager::GetQuestCoolTime( int code ) const
{
	time_t tCoolTime = 0;

	QuestCoolTimeInfo *pQuestCoolTime = NULL;
	if( m_hsQuestCoolTime.lookup( code, pQuestCoolTime ) )
		tCoolTime = pQuestCoolTime->cool_time;

	return tCoolTime;
}

const time_t StructQuestManager::GetRemainQuestCoolTime( int code, time_t tCurrent ) const
{
	time_t tCoolTime = 0;

	QuestCoolTimeInfo *pQuestCoolTime = NULL;
	if( m_hsQuestCoolTime.lookup( code, pQuestCoolTime ) )
		tCoolTime = pQuestCoolTime->cool_time;

	if( tCurrent < tCoolTime )
		return tCoolTime - tCurrent;

	return 0;
}