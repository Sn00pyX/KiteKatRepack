
#include <sstream>

#include <toolkit/XEnv.h>
#include <toolkit/XStringUtil.h>
#include <dump/XExceptionHandler.h>
#include <mmo/ArRegion.h>

#include "LogClient/LogClient.h"
#include "ErrorCode/ErrorCode.h"

#include "InstanceDungeonManager.h"
#include "ChannelManager.h"
#include "NPCProc.h"
#include "GameProc.h"
#include "StructPlayer.h"
#include "StructSummon.h"
#include "StructMonster.h"
#include "PartyManager.h"
#include "LuaVM.h"
#include "ThreadPlayerHelper.h"


extern __declspec( thread ) XSEH::THREAD_INFO s_ThreadInfo;

struct MapMonsterPtrEqual
{
	const StructMonster* _ptr ;
	MapMonsterPtrEqual(const StructMonster* ptr):_ptr(ptr){}
	bool operator() (const std::pair<int,StructMonster*>& left)
	{
		return left.second == _ptr;
	}
};

struct MapMonsterResIdEqual
{
	const int _respawn_id ;
	MapMonsterResIdEqual(const int respawn_id):_respawn_id(respawn_id){}
	bool operator() (const std::pair<int,StructMonster*>& left)
	{
		return left.second->GetInstanceRespawnID() == _respawn_id;
	}
};


struct QuitFunctor : public PartyManager::PartyFunctor
{
	QuitFunctor()
	{}

	virtual bool operator()( AR_HANDLE handle )
	{
		StructPlayer::iterator itPlayer = StructPlayer::get( handle );
		StructPlayer * pPlayer = (*itPlayer);

		if( !pPlayer )
			return false;

		ArPosition pos;
		pPlayer->GetPositionOnEnterInstanceGame( &pos );

		unsigned char nLayer = 0;

		int current_channel = ChannelManager::GetChannelId( pPlayer->GetX(), pPlayer->GetY() );
		int target_channel = ChannelManager::GetChannelId( pos.x, pos.y );

		if( current_channel && current_channel == target_channel )
		{
			nLayer = pPlayer->GetLayer();
		}
		else if( target_channel )
		{
			nLayer = ChannelManager::GetProperLayer( pos.x, pos.y );
		}

		// 파티 락 걸린 상태로 호출되기도 하는 곳이므로 지역락 걸지 않음
		pPlayer->PendWarp( pos.GetX(), pos.GetY(), nLayer );
		ArcadiaServer::Instance().SetObjectPriority( pPlayer, ArSchedulerObject::UPDATE_PRIORITY_HIGHEST );

		return true;
	}
};

// InstanceDungeonManager::InstanceDungeonType
void InstanceDungeonManager::InstanceDungeonType::onMonsterDelete( StructMonster * pMonster )
{
	int nRespawnID = pMonster->GetInstanceRespawnID();

	AR_TIME t = GetArTime();

	const GameContent::INSTANCE_DUNGEON_MONSTER_RESPAWN_INFO * pRespawnInfo = NULL;
	for( std::vector< const GameContent::INSTANCE_DUNGEON_MONSTER_RESPAWN_INFO * >::const_iterator itRespawn = pInstanceDungeonTypeBase->vRespawnInfo.begin() ;
		itRespawn != pInstanceDungeonTypeBase->vRespawnInfo.end() ; ++itRespawn )
	{
		if( (*itRespawn)->nID != nRespawnID )
			continue;

		pRespawnInfo = (*itRespawn);
		break;
	}

	{
		// csInstanceDungeonType 락을 걸어둔 상태로는 스크립트를 호출할 수 없으며(데드락),
		// 해당 락을 풀고 csMonsterDelete 같은 별도의 락 없이 스크립트를 실행하면 스크립트 안에서 GetAliveInstanceDungeonRespawnGroupMonsterCount를
		// 호출하기 전에 다른 쓰레드에서 onMonsterDelete를 호출해 mapRespawnedMonster.erase를 실행할 수 있으므로
		// 모든 몬스터가 한 방에 몰살당하면 각 몬스터가 동시에 onMonsterDelete를 실행하고 pRespawnInfo->szScriptOnDead 에서
		// GetAliveInstanceDungeonRespawnGroupMonsterCount를 호출할 때마다 0 마리 남았다는 결과만 반복해서 나온다거나 하게 됨.
		// csMonsterDelete를 사용함으로써 pRespawnInfo->szScriptOnDead 에서 GetAliveInstanceDungeonRespawnGroupMonsterCount를 호출할 경우
		// 모든 몬스터가 한 방의 광역 스킬에 죽었어도 남은 몬스터 수가 1씩 감소한 값을 리턴하게 됨.
		// 단, 이로 인해 csMonsterDelete 를 거는 부분은 모두 직렬화되어 광역 스킬로 한 번에 많은 몬스터를 죽일 경우
		// 병목 현상이 순간적으로 심하게 발생할 수 있음.
		THREAD_SYNCHRONIZE( csMonsterDelete );

		ARCADIA_LOCK( pInstanceDungeonInfo->LockWholeDungeonArea( nInstanceNo ) );

		std::vector< PlayerBasicInfoForLog > vPlayerBasicInfoList;

		{
			THREAD_SYNCHRONIZE1( csInstanceDungeonType );

			std::multimap< int, StructMonster * >::iterator it = std::find_if( mapRespawnedMonster.begin(), mapRespawnedMonster.end(), MapMonsterPtrEqual(pMonster) );

			// 이미 삭제 처리 된 몬스터면 리젠 진행시키지 않음(사냥 진행 중인데 사망 처리가 2번 들어왔거나 리젠만 되고 vRespawnedMonster에 추가되지 않았거나... -_ -?)
			if( it == mapRespawnedMonster.end() )
			{
				assert( 0 );
				return;
			}

			mapRespawnedMonster.erase( it );

			// 로그 작성을 위해 던전 내 참여 중이었던 유저들의 Gold/Exp/JP를 보관
			for( std::vector< StructPlayer * >::const_iterator itPlayer = vPlayerList.begin() ; itPlayer != vPlayerList.end() ; ++itPlayer )
			{
				vPlayerBasicInfoList.push_back( PlayerBasicInfoForLog( (*itPlayer) ) );
			}
		}

		// 몬스터 사망 시 실행될 스크립트 호출
		// * 시간제로 소환되었던 몬스터가 시간 제한에 의해 사라지는 경우에도 이 스크립트가 호출됨
		//   예를 들면 초강력 무적 몬스터로부터 1분간 살아남아라! 같은 미션을 주고 몬스터 죽는 스크립트가 실행되면 완료라든가
		//   근데 이런 기능을 사용하려면 스크립트나 스킬로 몬스터를 리젠시킬 때 리스폰 ID 번호를 할당해주는 기능이 필요한데
		//   아직 구현되어 있지 않음. 해당 기능 구현 시에는 DB에 입력된 데이터에 해당하는 리스폰 ID 번호를 발급해도
		//   다시 리젠되도록 하는 처리는 발생하지 않으므로(이 함수 바로 아랫 부분에 있음) 문제 없음.
		//   DB에 입력된 리스폰 데이터에 없는 별도의 스크립트를 실행하게 하려면 임시로 새로운
		//   GameContent::INSTANCE_DUNGEON_MONSTER_RESPAWN_INFO 인스턴스를 어딘가에 등록하고 시간제로 소환된 몬스터의
		//   리스폰 ID를 이용해 해당 인스턴스 포인터를 찾는 처리를 onMonsterDelete 맨 위에 해줘야 함.
		//   또한, 반복해서 자동으로 리젠되지 않도록 하기 위해 GameContent::INSTANCE_DUNGEON_MONSTER_RESPAWN_INFO::nPeriod를
		//   0으로 설정해 둬야 함
		if( pRespawnInfo && pRespawnInfo->szScriptOnDead[ 0 ] )
		{
			// 여기는 그냥 대표 유저(첫번째 유저)를 대상으로 설정한다.
			StructPlayer* pFirstPlayer = NULL;
			if( vPlayerList.empty() == false )
			{
				pFirstPlayer = vPlayerList.front();
			}

			ThreadPlayerHelper TPHepler( pFirstPlayer );

			char szScriptBuffer[ 256 ];
			s_strcpy( szScriptBuffer, _countof( szScriptBuffer ), pRespawnInfo->szScriptOnDead );

			char szElementBuffer[ 32 ];
			s_sprintf( szElementBuffer, _countof( szElementBuffer ), "%d", (int)pMonster->GetX() );
			XStringUtil::Replace( szScriptBuffer, _countof( szScriptBuffer ), "#@pos_x@#", szElementBuffer );
			s_sprintf( szElementBuffer, _countof( szElementBuffer ), "%d", (int)pMonster->GetY() );
			XStringUtil::Replace( szScriptBuffer, _countof( szScriptBuffer ), "#@pos_y@#", szElementBuffer );
			s_sprintf( szElementBuffer, _countof( szElementBuffer ), "%d", (int)pMonster->GetLayer() );
			XStringUtil::Replace( szScriptBuffer, _countof( szScriptBuffer ), "#@pos_layer@#", szElementBuffer );		
			s_sprintf( szElementBuffer, _countof( szElementBuffer ), "%u", pMonster->GetHandle() );
			XStringUtil::Replace( szScriptBuffer, _countof( szScriptBuffer ), "#@monster_handle@#", szElementBuffer );

			LUA()->RunString( szScriptBuffer );

			LOG::Log11N4S( LM_INSTANCE_DUNGEON_PROCESS, 0, 0, pInstanceDungeonInfo->pInstanceDungeonBase->nID, pInstanceDungeonTypeBase->nID, nInstanceNo, 0, 0, 0, 0, 0, 0,
				"", 0, "", 0, szScriptBuffer, LOG::STR_NTS, "MONSTER_KILL", LOG::STR_NTS );

			// Gold/Exp/JP 변동 유저들에 대한 로그 작성
			{
				THREAD_SYNCHRONIZE1( csInstanceDungeonType );

				for( std::vector< StructPlayer * >::const_iterator itPlayer = vPlayerList.begin() ; itPlayer != vPlayerList.end() ; ++itPlayer )
				{
					StructPlayer * pPlayer = (*itPlayer);

					// 이 검색 방식에서는 던전 내에 있다가 스크립트 처리 중에 없어졌거나, 혹은 없던 유저가 새로 생긴 경우는 로그가 남지 못 함
					for( std::vector< PlayerBasicInfoForLog >::iterator itPBI = vPlayerBasicInfoList.begin() ; itPBI != vPlayerBasicInfoList.end() ; ++itPBI )
					{
						const PlayerBasicInfoForLog & prevPBI = (*itPBI);
						if( prevPBI != pPlayer )
							continue;

						PlayerBasicInfoForLog currentPBI( pPlayer );
						if( prevPBI != currentPBI )
						{
							LOG::Log11N4S( LM_INSTANCE_DUNGEON_PLAYER_INFO_CHANGE, pPlayer->GetAccountID(), pPlayer->GetPlayerUID(), pInstanceDungeonInfo->pInstanceDungeonBase->nID, pInstanceDungeonTypeBase->nID, nInstanceNo,
								prevPBI.gold.GetRawData(), currentPBI.gold.GetRawData(), prevPBI.exp, currentPBI.exp, prevPBI.jp, currentPBI.jp,
								pPlayer->GetAccountName(), LOG::STR_NTS, pPlayer->GetName(), LOG::STR_NTS, szScriptBuffer, LOG::STR_NTS, "MONSTER_KILL", LOG::STR_NTS );
						}

						vPlayerBasicInfoList.erase( itPBI );
						break;
					}
				}
			}
		}
	}

	// 시간제로 소환되었던 몬스터가 시간 제한에 의해 사라지는 경우에는 리젠 예약 처리를 하지 않음
	if( pMonster->IsLifeTimeOver() )
		return;

	// 몬스터 리젠 예약(스킬에 의해 리젠되어 리젠 정보 ID 값이 설정되지 않은 몬스터는 pRespawnInfo == NULL 이어서 리젠 예약되지 않음)
	if( pRespawnInfo && pRespawnInfo->nPeriod )
	{
		THREAD_SYNCHRONIZE( csInstanceDungeonType );

		vPendedMonsterRespawn.push_back( PendedMonsterRespawnInfo( pRespawnInfo, t + pRespawnInfo->nPeriod ) );
	}
}

void InstanceDungeonManager::InstanceDungeonType::clearMonsters()
{
	// 리젠 대기 중인 몬스터 목록 제거
	vPendedMonsterRespawn.clear();

	// 몬스터 제거 처리(지역 락 필요)
	for( std::multimap< int , StructMonster * >::iterator itMonster = mapRespawnedMonster.begin() ; itMonster != mapRespawnedMonster.end() ; /* 루프에서 ++itMonster 처리 */ )
	{
		StructMonster* pMonster = itMonster->second;
		if( pMonster->IsEnable() )
		{
			pMonster->SetDeleteHandler( NULL );

			// 더이상의 스케쥴러 요청을 무시
			pMonster->Disable();

			// 월드에서 제거한다.
			if( pMonster->IsInWorld() )
			{
				RemoveMonsterFromWorld( pMonster );
			}

			// object delete 요청
			ArcadiaServer::Instance().DeleteObject( pMonster );

			itMonster = mapRespawnedMonster.erase( itMonster );
		}
		else
		{
			++itMonster;
		}
	}
}

void InstanceDungeonManager::InstanceDungeonType::onFieldPropDelete( StructFieldProp * pProp )
{
	THREAD_SYNCHRONIZE( csInstanceDungeonType );

	std::vector< StructFieldProp * >::iterator itErase = std::find( vRespawnedStaticProp.begin(), vRespawnedStaticProp.end(), pProp );

	if( itErase != vRespawnedStaticProp.end() )
	{
		vRespawnedStaticProp.erase( itErase );

		if( pProp->GetFieldPropBase()->nRegenTime != 0 )
			vPendedHealingPropRespawn.push_back( PendedHealingPropRespawnInfo( pProp->GetRespawnInfo(), GetArTime() + pProp->GetFieldPropBase()->nRegenTime, true ) );

		return;
	}

	itErase = std::find( vRespawnedProp.begin(), vRespawnedProp.end(), pProp );

	if( itErase != vRespawnedProp.end() )
	{
		vRespawnedProp.erase( itErase );

		if( pProp->GetFieldPropBase()->nRegenTime != 0 )
			vPendedHealingPropRespawn.push_back( PendedHealingPropRespawnInfo( pProp->GetRespawnInfo(), GetArTime() + pProp->GetFieldPropBase()->nRegenTime, false ) );
		else
			delete pProp->GetRespawnInfo();

		return;
	}

	// 이미 삭제된 프랍이면 아무 처리 안 함(삭제처리가 2번 일어났거나 리젠되기만 하고 vRespawnedStaticProp, vRespawnedProp에는 추가되지 않았거나 -_ -?)
	assert( 0 );
}

void InstanceDungeonManager::InstanceDungeonType::clearHealingProps()
{
	// 리젠 대기 중인 힐링 프랍 제거
	vPendedHealingPropRespawn.clear();

	// 힐링 프랍 제거 처리(지역 락 필요)
	for( std::vector< StructFieldProp * >::iterator itHealingProp = vRespawnedStaticProp.begin() ; itHealingProp != vRespawnedStaticProp.end() ; ++itHealingProp )
	{
		// 해당 프랍이 ProcDelete에서 다시 리젠되도록 vPendedHealingPropRespawn에 추가되지 않도록 DeleteHandler를 제거하고 삭제함
		(*itHealingProp)->SetDeleteHandler( NULL );

		// 유저가 사용함으로 인해서 RemoveObject/PendFreeFieldProp은 걸려 있지만 아직 onFieldPropDelete가 호출되지 않은 경우에는 아무것도 하지 않음
		if( !(*itHealingProp)->IsInWorld() )
			continue;

		ArcadiaServer::Instance().RemoveObject( (*itHealingProp) );
		StructFieldProp::PendFreeFieldProp( (*itHealingProp) );
	}

	vRespawnedStaticProp.clear();

	// 추가적으로 생성된 인던 내부의 프랍 제거 처리(지역 락 필요)
	for( std::vector< StructFieldProp * >::iterator itHealingProp = vRespawnedProp.begin() ; itHealingProp != vRespawnedProp.end() ; ++itHealingProp )
	{
		// 해당 프랍이 ProcDelete에서 다시 리젠되도록 vPendedHealingPropRespawn에 추가되지 않도록 DeleteHandler를 제거하고 삭제함
		(*itHealingProp)->SetDeleteHandler( NULL );

		// 유저가 사용함으로 인해서 RemoveObject/PendFreeFieldProp은 걸려 있지만 아직 onFieldPropDelete가 호출되지 않은 경우에는 아무것도 하지 않음
		if( !(*itHealingProp)->IsInWorld() )
			continue;

		ArcadiaServer::Instance().RemoveObject( (*itHealingProp) );
		StructFieldProp::PendFreeFieldProp( (*itHealingProp) );

		delete (*itHealingProp)->GetRespawnInfo();
	}

	vRespawnedProp.clear();
}

void InstanceDungeonManager::InstanceDungeonType::clearItems()
{
	// 비효율적인 방법이지만 아이템이 월드에 AddObject 될 때마다 인던 내부에 드랍되는 건지 체크하는 것보다는 부담이 적음(인던 종료될 때 한 번만 호출되므로)
	struct DroppedItemCollector : public ArRegionFunctor, ArObjectFunctor
	{
		// ArObjectFunctor
		virtual void operator()( ArObject *pObj ) const
		{
			if( !static_cast< GameObject * >( pObj )->IsItem() )
				return;

			vItemList.push_back( static_cast< StructItem * >( pObj ) );
		}

		// ArRegionFunctor
		virtual void operator()( const struct ArRegion * pRegion )
		{
			// DoEachStaticOjbect 안에서 ArObjectFunctor가 RemoveStaticObject를 호출하면
			// 해당 ArRegion의 m_vStaticObject 순회하던 게 박살나므로 포인터만 모음
			pRegion->DoEachStaticObject( *this );
		}

		mutable std::vector< StructItem * >		vItemList;
	} droppedItemCollector;

	ArcadiaServer::Instance().DoEachRegion( pInstanceDungeonInfo->pInstanceDungeonBase->nDungeonRegionLeft, pInstanceDungeonInfo->pInstanceDungeonBase->nDungeonRegionTop,
		pInstanceDungeonInfo->pInstanceDungeonBase->nDungeonRegionRight, pInstanceDungeonInfo->pInstanceDungeonBase->nDungeonRegionBottom, nInstanceNo, droppedItemCollector );

	for( std::vector< StructItem * >::iterator it = droppedItemCollector.vItemList.begin() ; it != droppedItemCollector.vItemList.end() ; ++it )
	{
		StructItem * pItem = (*it);

		// 월드에서 제거(ItemCollector::onProcess에서 삭제 중인 아이템이었을 경우 실패함)
		if( !RemoveItemFromWorld( pItem ) )
			continue;

		StructItem::PendFreeItem( pItem );
	}
}

const unsigned short InstanceDungeonManager::InstanceDungeonType::joinInstance( struct StructPlayer * pPlayer )
{
	if( std::find( vPlayerList.begin(), vPlayerList.end(), pPlayer ) != vPlayerList.end() )
	{
		assert( 0 );
		return RESULT_ALREADY_EXIST;
	}

	vPlayerList.push_back( pPlayer );
	++nPlayerCount;

	tDestructionTime = 0;

	return RESULT_SUCCESS;
}

const unsigned short InstanceDungeonManager::InstanceDungeonType::leaveInstance( struct StructPlayer * pPlayer )
{
	std::vector< StructPlayer * >::iterator it = std::find( vPlayerList.begin(), vPlayerList.end(), pPlayer );
	if( it == vPlayerList.end() )
	{
		assert( 0 );
		return RESULT_NOT_EXIST;
	}

	vPlayerList.erase( it );
	--nPlayerCount;

	assert( nPlayerCount >= 0 );

	// 적어도 방이 사라지지 않는 문제를 피하기 위해 우선 없앤다.
	if( nPlayerCount <= 0 )
		tDestructionTime = GetArTime();

	return RESULT_SUCCESS;
}

const unsigned short InstanceDungeonManager::InstanceDungeonType::beginInstance( struct StructPlayer * pPlayer )
{
	// 필요 아이템 소모
	// 우선 방을 만드는 사람만 소모를 한다
	__int64 nItemCount = 0;

	if( pInstanceDungeonTypeBase->nNeedItemCode )
	{
		StructItem *pItem = pPlayer->FindItem( pInstanceDungeonTypeBase->nNeedItemCode );
		nItemCount = pItem ? pItem->GetCount() : 0;

		if( !pItem || !pPlayer->EraseItem( pItem, pInstanceDungeonTypeBase->nNeedItemCount ) )
			return RESULT_NOT_ACTABLE;
	}

	// 몬스터 리젠 시키고~
	for( std::vector< const GameContent::INSTANCE_DUNGEON_MONSTER_RESPAWN_INFO * >::const_iterator itRespawn = pInstanceDungeonTypeBase->vRespawnInfo.begin() ; itRespawn != pInstanceDungeonTypeBase->vRespawnInfo.end() ; ++itRespawn )
	{
		// 리젠 조건이 외부 제어에 의한 경우 패스
		if( (*itRespawn)->bRespawnControl )
		{
			continue;
		}

		for( int nIndex = 0 ; nIndex < (*itRespawn)->nCount ; ++nIndex )
		{
			int nRespawnX;
			int nRespawnY;

			int nRespawnTryCount = 0;

			do
			{
				nRespawnX = XRandom( static_cast< int >( (*itRespawn)->bxArea.GetLeft() ), static_cast< int >( (*itRespawn)->bxArea.GetRight() ) );
				nRespawnY = XRandom( static_cast< int >( (*itRespawn)->bxArea.GetTop() ), static_cast< int >( (*itRespawn)->bxArea.GetBottom() ) );

				if( ++nRespawnTryCount > 10 )
				{
					FILELOG( "Unable to respawn monster in instance dungeon. InstanceDungeonID(%d), InstanceID(%d), RespawnID(%d)", pInstanceDungeonInfo->pInstanceDungeonBase->nID, pInstanceDungeonTypeBase->nID, (*itRespawn)->nID );
					_cprint( "Unable to respawn monster in instance dungeon. InstanceDungeonID(%d), InstanceID(%d), RespawnID(%d)\n", pInstanceDungeonInfo->pInstanceDungeonBase->nID, pInstanceDungeonTypeBase->nID, (*itRespawn)->nID );

					nRespawnTryCount = 0;
					break;
				}
			}
			while( GameContent::IsBlocked( nRespawnX, nRespawnY ) );

			if( !nRespawnTryCount )
				continue;

			StructMonster * pMob = respawnMonster( nRespawnX, nRespawnY, nInstanceNo, (*itRespawn)->nMonsterID, (*itRespawn)->bWandering, 0, this, true, nDifficulty );
			if( !pMob )
			{
				FILELOG( "Unknown monster is to be respawned in instance dungeon. InstanceDungeonID(%d), InstanceID(%d), RespawnID(%d)", pInstanceDungeonInfo->pInstanceDungeonBase->nID, pInstanceDungeonTypeBase->nID, (*itRespawn)->nID );
				_cprint( "Unknown monster is to be respawned in instance dungeon. InstanceDungeonID(%d), InstanceID(%d), RespawnID(%d)\n", pInstanceDungeonInfo->pInstanceDungeonBase->nID, pInstanceDungeonTypeBase->nID, (*itRespawn)->nID );

				continue;
			}

			pMob->SetInstanceRespawnID( (*itRespawn)->nID );

			mapRespawnedMonster.insert( std::pair< int , StructMonster* >( (*itRespawn)->nRespawnGroup, pMob ) );
		}
	}

	// 프랍 리젠 시키고~
	for( std::vector< const GameContent::FIELD_PROP_RESPAWN_INFO * >::const_iterator itHealingProp = pInstanceDungeonTypeBase->vHealingPropInfo.begin() ; itHealingProp != pInstanceDungeonTypeBase->vHealingPropInfo.end() ; ++itHealingProp )
	{
		StructFieldProp * pProp = NULL;
		{
			pProp = StructFieldProp::Create( this, (*itHealingProp), (*itHealingProp)->x, (*itHealingProp)->y, nInstanceNo );
		}

		if( pProp )
		{
			vRespawnedStaticProp.push_back( pProp );
		}
	}

	LOG::Log11N4S( LM_INSTANCE_DUNGEON_CREATE, pPlayer->GetAccountID(), pPlayer->GetSID(), pInstanceDungeonInfo->pInstanceDungeonBase->nID, pInstanceDungeonTypeBase->nID, nInstanceNo, nKey, 0, 0, pInstanceDungeonTypeBase->nNeedItemCode, pInstanceDungeonTypeBase->nNeedItemCount, nItemCount - pInstanceDungeonTypeBase->nNeedItemCount,
		pPlayer->GetAccountName(), LOG::STR_NTS, pPlayer->GetName(), LOG::STR_NTS, "", 0, "", 0 );

	if( GameRule::bLogVulcanusDungeon && pInstanceDungeonInfo->pInstanceDungeonBase->nID == GameRule::DUNGEON_VULCANUS_ID )
	{
		StructSummon *pMainSummon = pPlayer->GetMainSummon();
		StructSummon *pSubSummon = pPlayer->GetSubSummon();

		LOG::Log11N4S( LM_VULCANUS_DUNGEON_BEGIN, pPlayer->GetAccountID(), pPlayer->GetSID(), pInstanceDungeonInfo->pInstanceDungeonBase->nID, pInstanceDungeonTypeBase->nID, nInstanceNo, pPlayer->GetJobId(), pPlayer->GetLevel(), pMainSummon ? pMainSummon->GetSummonCode() : 0, pMainSummon ? pMainSummon->GetLevel() : 0, pSubSummon ? pSubSummon->GetSummonCode() : 0, pSubSummon ? pSubSummon->GetLevel() : 0,
			pPlayer->GetAccountName(), LOG::STR_NTS, pPlayer->GetName(), LOG::STR_NTS, "", 0, "", 0 );
	}

	return RESULT_SUCCESS;
}

const unsigned short InstanceDungeonManager::InstanceDungeonType::endInstance()
{
	if( nPlayerCount > 0 )
	{
		assert( 0 );
		return RESULT_NOT_ACTABLE;
	}

	// 리젠 대기 중인 몬스터 데이터 제거
	{
		THREAD_SYNCHRONIZE( pInstanceDungeonInfo->csPendedRespawn );

		for( std::vector< GameContent::PENDED_RESPAWN_INFO * >::iterator itRespawn = pInstanceDungeonInfo->vPendedRespawn.begin() ; itRespawn != pInstanceDungeonInfo->vPendedRespawn.end() ; /* 루프에서 ++itRespawn 처리 */ )
		{
			if( (*itRespawn)->layer == nInstanceNo )
			{
				delete (*itRespawn);
				itRespawn = pInstanceDungeonInfo->vPendedRespawn.erase( itRespawn );
			}
			else
				++itRespawn;
		}
	}

	// 리젠 대기 중인 프랍 데이터 제거
	{
		THREAD_SYNCHRONIZE( pInstanceDungeonInfo->csPendedRespawnProp );

		for( std::vector< GameContent::FIELD_PROP_RESPAWN_INFO * >::iterator itRespawn = pInstanceDungeonInfo->vPendedRespawnProp.begin() ; itRespawn != pInstanceDungeonInfo->vPendedRespawnProp.end() ; /* 루프에서 ++itRespawn 처리 */ )
		{
			if( (*itRespawn)->layer == nInstanceNo )
			{
				delete (*itRespawn);
				itRespawn = pInstanceDungeonInfo->vPendedRespawnProp.erase( itRespawn );
			}
			else
				++itRespawn;
		}
	}

	// 통계 및 분석을 위한 불카누스 던전 전용 로그, 몬스터의 생존 여부를 알기 위해 방 폐쇄 이전에 로그를 남긴다.
	if( GameRule::bLogVulcanusDungeon && pInstanceDungeonInfo->pInstanceDungeonBase->nID == GameRule::DUNGEON_VULCANUS_ID )
	{
		LOG::Log11N4S( LM_VULCANUS_DUNGEON_END, nOwnerAccountID, nOwnerUID, pInstanceDungeonInfo->pInstanceDungeonBase->nID, pInstanceDungeonTypeBase->nID, nInstanceNo, atoi( getFlag( "Vul1" ).c_str() ), atoi( getFlag( "Vul2" ).c_str() ), atoi( getFlag( "Vul3" ).c_str() ), !getAliveRespawnGroupMonsterCount( 20013 ), 0, 0,
			szOwnerAccountName, LOG::STR_NTS, szOwnerName, LOG::STR_NTS, "", 0, "", 0 );
	}

	// 리젠되어 있는 몬스터/프랍/아이템 제거
	clearMonsters();
	clearHealingProps();
	clearItems();

	LOG::Log11N4S( LM_INSTANCE_DUNGEON_DESTROY, nOwnerAccountID, nOwnerUID, pInstanceDungeonInfo->pInstanceDungeonBase->nID, pInstanceDungeonTypeBase->nID, nInstanceNo, nKey, 0, 0, 0, 0, 0,
		szOwnerAccountName, LOG::STR_NTS, szOwnerName, LOG::STR_NTS, "", 0, "", 0 );

	return RESULT_SUCCESS;
}

const int InstanceDungeonManager::InstanceDungeonType::doEachPlayer( ArObjectFunctor & functor )
{
	for( std::vector< StructPlayer * >::iterator it = vPlayerList.begin() ; it != vPlayerList.end() ; ++it )
	{
		functor( (*it) );
	}

	return (int)vPlayerList.size();
}

void InstanceDungeonManager::InstanceDungeonType::procMonsterRespawn( const AR_TIME & tCurrent )
{
	for( std::vector< PendedMonsterRespawnInfo >::iterator itRespawn = vPendedMonsterRespawn.begin() ; itRespawn != vPendedMonsterRespawn.end() ; /* 루프에서 ++itRespawn 처리 */ )
	{
		if( (*itRespawn).second > tCurrent )
		{
			++itRespawn;
			continue;
		}

		for( int nIndex = 0 ; nIndex < (*itRespawn).first->nCount ; ++nIndex )
		{
			int nRespawnX;
			int nRespawnY;

			int nRespawnTryCount = 0;

			do
			{
				nRespawnX = XRandom( static_cast< int >( (*itRespawn).first->bxArea.GetLeft() ), static_cast< int >( (*itRespawn).first->bxArea.GetRight() ) );
				nRespawnY = XRandom( static_cast< int >( (*itRespawn).first->bxArea.GetTop() ), static_cast< int >( (*itRespawn).first->bxArea.GetBottom() ) );

				if( ++nRespawnTryCount > 10 )
				{
					FILELOG( "Unable to respawn monster in instance dungeon. InstanceDungeonID(%d), InstanceID(%d), RespawnID(%d)", pInstanceDungeonInfo->pInstanceDungeonBase->nID, pInstanceDungeonTypeBase->nID, (*itRespawn).first->nID );
					_cprint( "Unable to respawn monster in instance dungeon. InstanceDungeonID(%d), InstanceID(%d), RespawnID(%d)\n", pInstanceDungeonInfo->pInstanceDungeonBase->nID, pInstanceDungeonTypeBase->nID, (*itRespawn).first->nID );

					nRespawnTryCount = 0;
					break;
				}
			}
			while( GameContent::IsBlocked( nRespawnX, nRespawnY ) );

			if( !nRespawnTryCount )
				continue;

			StructMonster * pMob = respawnMonster( nRespawnX, nRespawnY, nInstanceNo, (*itRespawn).first->nMonsterID, (*itRespawn).first->bWandering, 0, this, true, nDifficulty );
			if( !pMob )
			{
				FILELOG( "Unknown monster is to be respawned in instance dungeon. InstanceDungeonID(%d), InstanceID(%d), RespawnID(%d)", pInstanceDungeonInfo->pInstanceDungeonBase->nID, pInstanceDungeonTypeBase->nID, (*itRespawn).first->nID );
				_cprint( "Unknown monster is to be respawned in instance dungeon. InstanceDungeonID(%d), InstanceID(%d), RespawnID(%d)\n", pInstanceDungeonInfo->pInstanceDungeonBase->nID, pInstanceDungeonTypeBase->nID, (*itRespawn).first->nID );

				continue;
			}

			pMob->SetInstanceRespawnID( (*itRespawn).first->nID );

			mapRespawnedMonster.insert( std::pair< int , StructMonster* >( (*itRespawn).first->nRespawnGroup, pMob ) );
		}

		itRespawn = vPendedMonsterRespawn.erase( itRespawn );
	}
}

void InstanceDungeonManager::InstanceDungeonType::procHealingPropRespawn( const AR_TIME & tCurrent )
{
	for( std::vector< PendedHealingPropRespawnInfo >::iterator itRespawn = vPendedHealingPropRespawn.begin() ; itRespawn != vPendedHealingPropRespawn.end() ; /* 루프에서 ++itRespawn 처리 */ )
	{
		if( (*itRespawn).tTime > tCurrent )
		{
			++itRespawn;
			continue;
		}

		StructFieldProp * pProp = NULL;
		{
			pProp = StructFieldProp::Create( this, (*itRespawn).pInfo, (*itRespawn).pInfo->x, (*itRespawn).pInfo->y, nInstanceNo );
		}

		if( pProp )
		{
			if( (*itRespawn).bStatic )
				vRespawnedStaticProp.push_back( pProp );
			else
				vRespawnedProp.push_back( pProp );
		}

		itRespawn = vPendedHealingPropRespawn.erase( itRespawn );
	}
}

void InstanceDungeonManager::InstanceDungeonType::setFlag( const std::string & strName , const std::string & strValue )
{
	m_mapFlags[ strName ] = strValue;
}

const std::string InstanceDungeonManager::InstanceDungeonType::getFlag( const std::string & strName ) const
{
	std::map< std::string, std::string >::const_iterator iter = m_mapFlags.find( strName );

	if( iter == m_mapFlags.end() )
		return "";

	return iter->second;
}

int InstanceDungeonManager::InstanceDungeonType::getAliveRespawnGroupMonsterCount( const int nRespawnGroup ) const
{
	std::multimap< int, StructMonster * >::const_iterator iter_first = mapRespawnedMonster.lower_bound( nRespawnGroup );
	std::multimap< int, StructMonster * >::const_iterator iter_last = mapRespawnedMonster.upper_bound( nRespawnGroup );
	std::multimap< int, StructMonster * >::const_iterator iter_end = mapRespawnedMonster.end();

	int nResultCount = 0;
	while( iter_first != iter_end && iter_first != iter_last )
	{
		++nResultCount;
		++iter_first;
	}

	return nResultCount;
}

const unsigned char InstanceDungeonManager::InstanceDungeonInfo::GetNewInstanceNo() const
{
	unsigned char nInstanceNo = 1;

	while( mapInstanceNo.find( nInstanceNo ) != mapInstanceNo.end() && nInstanceNo <= MAX_INSTANCE )
		++nInstanceNo;

	return nInstanceNo;
}

void InstanceDungeonManager::InstanceDungeonInfo::ProcPendedMonsterRespawn()
{
	// [지역 락 -> csPendedRespawn / csInstanceDungeonType -> InstanceDungeonType::csInstanceDungeonType -> 지역 락] 지역 락으로 인한 데드락 발생 방지용 락 전략
	std::vector< GameContent::PENDED_RESPAWN_INFO * > _vPendedRespawn;
	{
		THREAD_SYNCHRONIZE( csPendedRespawn );

		if( vPendedRespawn.empty() )
			return;

		_vPendedRespawn.swap( vPendedRespawn );
	}

	THREAD_SYNCHRONIZE( csInstanceDungeonType );

	for( std::vector< GameContent::PENDED_RESPAWN_INFO * >::const_iterator itRespawn = _vPendedRespawn.begin() ; itRespawn != _vPendedRespawn.end() ; ++itRespawn )
	{
		GameContent::PENDED_RESPAWN_INFO * pRespawn = (*itRespawn);

		for( std::map< int, InstanceDungeonType * >::iterator itInstance = mapInstanceDungeonType.begin() ; itInstance != mapInstanceDungeonType.end() ; ++itInstance )
		{
			InstanceDungeonType * pInstance = (*itInstance).second;

			if( pInstance->nInstanceNo != pRespawn->layer )
				continue;

			THREAD_SYNCHRONIZE( pInstance->csInstanceDungeonType );

			for( int nCountIdx = 0 ; nCountIdx < pRespawn->respawnCount ; ++nCountIdx )
			{
				// 랜덤 위치 설정
				AR_UNIT x, y;
				bool bValidPos = true;
				unsigned short try_cnt = 0;
				do
				{
					x = XRandom( pRespawn->x - 60, pRespawn->x + 60 );
					y = XRandom( pRespawn->y - 60, pRespawn->y + 60 );

					if( ++try_cnt > 300 )
					{
						bValidPos = false;
						break;
					}
				} while( GameContent::CollisionToLine( pRespawn->x, pRespawn->y, x, y ) );

				if( !bValidPos )
				{
					x = pRespawn->x;
					y = pRespawn->y;

					if( GameContent::IsBlocked( x, y ) )
					{
						FILELOG( "Unable to respawn monster in battle arena. InstanceID(%d), x(%d), y(%d), layer(%d), MonsterID(%d)", pInstanceDungeonBase->nID, pRespawn->x, pRespawn->y, pRespawn->layer, pRespawn->monsterId );
						_cprint( "Unable to respawn monster in battle arena. InstanceID(%d), x(%d), y(%d), layer(%d), MonsterID(%d)\n", pInstanceDungeonBase->nID, pRespawn->x, pRespawn->y, pRespawn->layer, pRespawn->monsterId );
					}
				}

				{
					StructMonster * pMob = respawnMonster( x, y, pRespawn->layer, pRespawn->monsterId, true, 0, pInstance, true, pInstance->nDifficulty );
					if( !pMob )
					{
						FILELOG( "Unknown monster is to be respawned in instance dungeon by pending. InstanceID(%d), x(%d), y(%d), layer(%d), MonsterID(%d)", pInstanceDungeonBase->nID, pRespawn->x, pRespawn->y, pRespawn->layer, pRespawn->monsterId );
						_cprint( "Unknown monster is to be respawned in instance dungeon by pending. InstanceID(%d), x(%d), y(%d), layer(%d), MonsterID(%d)\n", pInstanceDungeonBase->nID, pRespawn->x, pRespawn->y, pRespawn->layer, pRespawn->monsterId );

						break;
					}

					if( pRespawn->initialEnemy )
						pMob->AddHate( pRespawn->initialEnemy, 1 );

					pMob->SetLifeTime( pRespawn->lifeTime );

					//보스 몬스터가 스킬로 소환 하는 몬스터들은 키값을 0으로 설정한다.
					pInstance->mapRespawnedMonster.insert( std::pair< int , StructMonster* >( /*pRespawn->respawnGroup*/0 , pMob ) );
					//pInstance->vRespawnedMonster.push_back( pMob );
				}
			}
		}

		delete pRespawn;
	}
}

void InstanceDungeonManager::InstanceDungeonInfo::ProcPendedPropRespawn()
{
	// [지역 락 -> csPendedRespawnProp / csInstanceDungeonType -> InstanceDungeonType::csInstanceDungeonType -> 지역 락] 지역 락으로 인한 데드락 발생 방지용 락 전략
	std::vector< GameContent::FIELD_PROP_RESPAWN_INFO * > _vPendedRespawnProp;
	{
		THREAD_SYNCHRONIZE( csPendedRespawnProp );

		if( vPendedRespawnProp.empty() )
			return;

		_vPendedRespawnProp.swap( vPendedRespawnProp );
	}

	THREAD_SYNCHRONIZE( csInstanceDungeonType );

	for( std::vector< GameContent::FIELD_PROP_RESPAWN_INFO * >::const_iterator itRespawn = _vPendedRespawnProp.begin() ; itRespawn != _vPendedRespawnProp.end() ; ++itRespawn )
	{
		GameContent::FIELD_PROP_RESPAWN_INFO * pRespawn = (*itRespawn);

		for( std::map< int, InstanceDungeonType * >::iterator itInstance = mapInstanceDungeonType.begin() ; itInstance != mapInstanceDungeonType.end() ; ++itInstance )
		{
			InstanceDungeonType * pInstance = (*itInstance).second;

			if( pInstance->nInstanceNo != pRespawn->layer )
				continue;

			THREAD_SYNCHRONIZE( pInstance->csInstanceDungeonType );

			StructFieldProp * pProp = StructFieldProp::Create( pInstance, pRespawn, pRespawn->x, pRespawn->y, pInstance->nInstanceNo );

			if( pProp )
			{
				pInstance->vRespawnedProp.push_back( pProp );
			}
		}
	}
}

ArcadiaLock InstanceDungeonManager::InstanceDungeonInfo::LockWholeDungeonArea( short layer ) const
{
	// 데드락 발생 가능성 체크용
	assert( !csInstanceDungeonType.IsLockedByCurrentThread() &&
		!csPendedRespawn.IsLockedByCurrentThread() &&
		!csPendedRespawnProp.IsLockedByCurrentThread() );

	return ArcadiaServer::Instance().LockArea( pInstanceDungeonBase->nDungeonRegionLeft, pInstanceDungeonBase->nDungeonRegionTop,
		pInstanceDungeonBase->nDungeonRegionRight, pInstanceDungeonBase->nDungeonRegionBottom, layer );
}

// InstanceDungeonManager
InstanceDungeonManager::~InstanceDungeonManager()
{
	if( IsInitialized() )
		DeInit();

	 ClearInstanceDungeonInfo();
}

InstanceDungeonManager & InstanceDungeonManager::Instance()
{
	static InstanceDungeonManager _inst;

	return _inst;
}

bool InstanceDungeonManager::Init()
{
	if( IsInitialized() )
		return false;

	ArcadiaServer::Instance().SetObjectPriority( this, ArObject::UPDATE_PRIORITY_HIGH );

	return true;
}

bool InstanceDungeonManager::DeInit()
{
	if( !IsInitialized() )
		return false;

	ArcadiaServer::Instance().SetObjectPriority( this, ArObject::UPDATE_PRIORITY_IDLE );

	// 진행 중이던 인스턴스 던전이 있는 상태에서 종료 또는 뒷 처리를 해야할 경우 여기서 하면 됨
	for( std::vector< InstanceDungeonInfo * >::iterator it = m_vInstanceDungeonInfo.begin() ; it != m_vInstanceDungeonInfo.end() ; ++it )
	{
		// 리젠 대기 중인 몬스터 데이터 제거
		{
			THREAD_SYNCHRONIZE( (*it)->csPendedRespawn );

			for( std::vector< GameContent::PENDED_RESPAWN_INFO * >::const_iterator itRespawn = (*it)->vPendedRespawn.begin() ; itRespawn != (*it)->vPendedRespawn.end() ; ++itRespawn )
			{
				delete (*itRespawn);
			}

			(*it)->vPendedRespawn.clear();
		}

		// 리젠 대기 중인 프랍 데이터 제거
		{
			THREAD_SYNCHRONIZE( (*it)->csPendedRespawnProp );

			for( std::vector< GameContent::FIELD_PROP_RESPAWN_INFO * >::const_iterator itRespawn = (*it)->vPendedRespawnProp.begin() ; itRespawn != (*it)->vPendedRespawnProp.end(); ++itRespawn )
			{
				delete (*itRespawn);
			}

			(*it)->vPendedRespawnProp.clear();
		}

		// 삭제되어야 할 던전 정보를 즉시 삭제하기 위해서 모으는 벡터
		std::vector< InstanceDungeonType * > vPendedInstanceDungeonTypeToBeDeleted;
		{
			THREAD_SYNCHRONIZE( (*it)->csInstanceDungeonType );

			// 삭제 대기 중인 던전들을 즉시 삭제할 목록에 추가 후 대기 리스트 초기화
			vPendedInstanceDungeonTypeToBeDeleted.swap( (*it)->vPendedInstanceDungeonTypeToBeDeleted );
			(*it)->mapInstanceNo.clear();

			for( std::map< int, InstanceDungeonType * >::iterator itInstance = (*it)->mapInstanceDungeonType.begin() ; itInstance != (*it)->mapInstanceDungeonType.end() ; /* 루프에서 ++itInstance 처리 */ )
			{
				InstanceDungeonType * pInstance = (*itInstance).second;

				THREAD_SYNCHRONIZE( pInstance->csInstanceDungeonType );

				pInstance->endInstance();

				vPendedInstanceDungeonTypeToBeDeleted.push_back( (*itInstance).second );
				itInstance = (*it)->mapInstanceDungeonType.erase( itInstance );
			}
		}

		for( std::vector< InstanceDungeonType * >::const_iterator itInstance = vPendedInstanceDungeonTypeToBeDeleted.begin() ; itInstance != vPendedInstanceDungeonTypeToBeDeleted.end() ; ++itInstance )
		{
			// 삭제 전에 락을 한 번 걸었다 풀어서 다른 프로세스가 인스턴스 정보를 사용 중일 가능성을 낮춘 후에 삭제
			// 그래봐야 여기서 거는 락 때문에 기다리고 있는 쓰레드가 생기면 말짱 꽝이지만,
			// 어차피 서버 내려가고 있는데 그딴 거 알 게 뭐삼! 걍 지워!(서버 종료되면서 불필요한 덤프가 남는 주 원인이 될 수도 있음... ``;)
			{
				THREAD_SYNCHRONIZE( (*itInstance)->csInstanceDungeonType );
			}
			delete (*itInstance);
		}
	}

	return true;
}

const bool InstanceDungeonManager::IsInitialized() const
{
	return GetFinalPriority() != ArSchedulerObject::UPDATE_PRIORITY_IDLE;
}

const bool InstanceDungeonManager::ClearInstanceDungeonInfo()
{
	if( IsInitialized() )
		return false;

	for( std::vector< InstanceDungeonInfo * >::const_iterator it = m_vInstanceDungeonInfo.begin() ; it != m_vInstanceDungeonInfo.end() ; ++it )
	{
		// beginInstance에서 몬스터/프랍 리젠 정보를 사용하지만 ClearInstanceDungeonInfo가 호출되는 상황(DeInit)이면
		// beginInstance이 처리 중일리가 없으므로 락 안 걸고 삭제
		for( std::vector< const GameContent::INSTANCE_DUNGEON_TYPE_BASE * >::const_iterator itInstance = (*it)->pInstanceDungeonBase->vInstanceDungeonTypeBase.begin() ; itInstance != (*it)->pInstanceDungeonBase->vInstanceDungeonTypeBase.end() ; ++itInstance )
		{
			for( std::vector< const GameContent::INSTANCE_DUNGEON_MONSTER_RESPAWN_INFO * >::const_iterator itRespawn = (*itInstance)->vRespawnInfo.begin() ; itRespawn != (*itInstance)->vRespawnInfo.end() ; ++itRespawn )
			{
				delete (*itRespawn);
			}

			for( std::vector< const GameContent::FIELD_PROP_RESPAWN_INFO * >::const_iterator itHealingProp = (*itInstance)->vHealingPropInfo.begin() ; itHealingProp != (*itInstance)->vHealingPropInfo.end() ; ++itHealingProp )
			{
				delete (*itHealingProp);
			}

			delete (*itInstance);
		}

		delete (*it);
	}
	m_vInstanceDungeonInfo.clear();

	return true;
}

const bool InstanceDungeonManager::RegisterInstanceDungeonBase( const GameContent::INSTANCE_DUNGEON_BASE * pInstanceDungeonBase )
{
	assert( !IsInitialized() );

#ifdef _DEBUG
	// 디버그 모드일 때만 이미 등록된 인스턴스 던전 정보인지 확인
	for( std::vector< InstanceDungeonInfo * >::const_iterator it = m_vInstanceDungeonInfo.begin() ; it != m_vInstanceDungeonInfo.end() ; ++it )
	{
		if( (*it)->pInstanceDungeonBase->nID == pInstanceDungeonBase->nID )
			return false;
	}
#endif

	m_vInstanceDungeonInfo.push_back( new InstanceDungeonInfo( pInstanceDungeonBase ) );

	return true;
}

const InstanceDungeonManager::InstanceDungeonInfo * InstanceDungeonManager::findInstanceDungeonInfo( const ArPosition & pos ) const
{
	for( std::vector< InstanceDungeonInfo * >::const_iterator it = m_vInstanceDungeonInfo.begin() ; it != m_vInstanceDungeonInfo.end() ; ++it )
	{
		unsigned int nRegionX = pos.GetRX();
		unsigned int nRegionY = pos.GetRY();

		if( nRegionX >= (*it)->pInstanceDungeonBase->nDungeonRegionLeft && nRegionX <= (*it)->pInstanceDungeonBase->nDungeonRegionRight &&
			nRegionY >= (*it)->pInstanceDungeonBase->nDungeonRegionTop && nRegionY <= (*it)->pInstanceDungeonBase->nDungeonRegionBottom )
			return (*it);
	}

	return NULL;
}

const InstanceDungeonManager::InstanceDungeonInfo * InstanceDungeonManager::findInstanceDungeonInfo( int nInstanceDungeonID ) const
{
	for( std::vector< InstanceDungeonInfo * >::const_iterator it = m_vInstanceDungeonInfo.begin() ; it != m_vInstanceDungeonInfo.end() ; ++it )
	{
		if( (*it)->pInstanceDungeonBase->nID != nInstanceDungeonID )
			continue;

		return (*it);
	}

	return NULL;
}

InstanceDungeonManager::InstanceDungeonInfo * InstanceDungeonManager::findInstanceDungeonInfo( int nInstanceDungeonID )
{
	for( std::vector< InstanceDungeonInfo * >::iterator it = m_vInstanceDungeonInfo.begin() ; it != m_vInstanceDungeonInfo.end() ; ++it )
	{
		if( (*it)->pInstanceDungeonBase->nID != nInstanceDungeonID )
			continue;

		return (*it);
	}

	return NULL;
}

const InstanceDungeonManager::InstanceDungeonType * InstanceDungeonManager::findInstanceDungeonType( const InstanceDungeonInfo * pInstanceDungeonInfo, const unsigned char nLayer ) const
{
	typedef std::map< unsigned char, InstanceDungeonType * >::const_iterator mapIter;

	mapIter it = pInstanceDungeonInfo->mapInstanceNo.find( nLayer );

	if( it != pInstanceDungeonInfo->mapInstanceNo.end() )
	{
		return it->second;
	}

	return NULL;
}

InstanceDungeonManager::InstanceDungeonType * InstanceDungeonManager::findInstanceDungeonType( InstanceDungeonInfo * pInstanceDungeonInfo, const unsigned char nLayer )
{
	typedef std::map< unsigned char, InstanceDungeonType * >::iterator mapIter;

	mapIter it = pInstanceDungeonInfo->mapInstanceNo.find( nLayer );

	if( it != pInstanceDungeonInfo->mapInstanceNo.end() )
	{
		return it->second;
	}

	return NULL;
}

const int InstanceDungeonManager::GetInstanceDungeonID( const ArPosition & pos ) const
{
	const InstanceDungeonInfo * pInstanceDungeonInfo = findInstanceDungeonInfo( pos );

	return ( pInstanceDungeonInfo != NULL ) ? pInstanceDungeonInfo->pInstanceDungeonBase->nID : 0;
}

const int InstanceDungeonManager::GetInstanceDungeonTypeID( int nInstanceDungeonID, const unsigned char nLayer ) const
{
	const InstanceDungeonInfo * pInstanceDungeonInfo = findInstanceDungeonInfo( nInstanceDungeonID );

	if( !pInstanceDungeonInfo )
		return -1;

	THREAD_SYNCHRONIZE( pInstanceDungeonInfo->csInstanceDungeonType );

	const InstanceDungeonType * pInstanceDungeonType = findInstanceDungeonType( pInstanceDungeonInfo, nLayer );

	return ( pInstanceDungeonType != NULL ) ? pInstanceDungeonType->pInstanceDungeonTypeBase->nID : -1;
}

const bool InstanceDungeonManager::GetInstanceDungeonPosition( const int nInstanceDungeonID, ArPosition & pos ) const
{
	const InstanceDungeonInfo * pInstanceDungeonInfo = findInstanceDungeonInfo( nInstanceDungeonID );

	if( !pInstanceDungeonInfo )
		return false;

	pos = pInstanceDungeonInfo->pInstanceDungeonBase->posDungeon;
	return true;
}

const bool InstanceDungeonManager::SetInstanceDungeonTypeFlag( int nInstanceDungeonID, const unsigned char nLayer, const std::string & strName , const std::string & strValue )
{
	InstanceDungeonInfo * pInstanceDungeonInfo = findInstanceDungeonInfo( nInstanceDungeonID );

	if( !pInstanceDungeonInfo )
		return false;

	THREAD_SYNCHRONIZE( pInstanceDungeonInfo->csInstanceDungeonType );

	InstanceDungeonType * pInstanceDungeonType = findInstanceDungeonType( pInstanceDungeonInfo, nLayer );

	if( !pInstanceDungeonType )
		return false;

	THREAD_SYNCHRONIZE1( pInstanceDungeonType->csInstanceDungeonType );

	pInstanceDungeonType->setFlag( strName, strValue );
	return true;
}

std::string InstanceDungeonManager::GetInstanceDungeonTypeFlag( int nInstanceDungeonID, const unsigned char nLayer, const std::string & strName ) const
{
	const InstanceDungeonInfo * pInstanceDungeonInfo = findInstanceDungeonInfo( nInstanceDungeonID );

	if( !pInstanceDungeonInfo )
		return "";

	THREAD_SYNCHRONIZE( pInstanceDungeonInfo->csInstanceDungeonType );

	const InstanceDungeonType * pInstanceDungeonType = findInstanceDungeonType( pInstanceDungeonInfo, nLayer );

	if( !pInstanceDungeonType )
		return "";

	THREAD_SYNCHRONIZE1( pInstanceDungeonType->csInstanceDungeonType );

	// InstanceDungeonType::getFlag 함수의 리턴 타입은 const std::string & 인 반면 이 함수의 리턴 타입은 std::string 임.
	// 이 함수에서 리턴되는 순간 인던 관련 모든 락은 풀리고 검색된 std::string 의 인스턴스는 메모리에서 삭제될 수도 있음.
	// 따라서 InstanceDungeonManager 내부에서 사용되는 함수인 InstanceDungeonType::getFlag는 락을 관리하여 참조자의 유효성을 보장할 수 있지만
	// 이 함수는 유효성을 보장하지 못하므로 찾아진 const std::string & 의 복사본을 리턴해야 함.
	// 또한 이 함수의 리턴값인 std::string을 호출자 측에서 참조자로 받아서 내용을 수정해서 사용하는 건 자유이므로(리턴값은 호출자 측 마음대로~) const를 안 붙임.
	return pInstanceDungeonType->getFlag( strName );
}

const int InstanceDungeonManager::GetInstanceDungeonDifficulty( const int nInstanceDungeonID, const unsigned char nLayer ) const
{
	const InstanceDungeonInfo * pInstanceDungeonInfo = findInstanceDungeonInfo( nInstanceDungeonID );
	if( !pInstanceDungeonInfo )
		return -1;

	THREAD_SYNCHRONIZE( pInstanceDungeonInfo->csInstanceDungeonType );
	const InstanceDungeonType * pType = findInstanceDungeonType( pInstanceDungeonInfo, nLayer );
	if( !pType )
		return -1;

	// 여기서 Difficulty란 인스턴스 던전의 nID를 말함. 네이밍을 좀 더 직관적으로 수정해야 될 듯
	// pType->nDifficulty 라는 값을 현재 안 쓰고 있다고 한다. DB에는 있는 값이니 컬럼 삭제와 함께 네이밍 리팩토링이 필요할 듯
	return pType->pInstanceDungeonTypeBase->nID;
}

const int InstanceDungeonManager::GetAliveInstanceDungeonRespawnGroupMonsterCount( int nInstanceDungeonID, const unsigned char nLayer, const int nRespawnGroup ) const
{
	const InstanceDungeonInfo * pInstanceDungeonInfo = findInstanceDungeonInfo( nInstanceDungeonID );

	if( !pInstanceDungeonInfo )
		return -1;

	THREAD_SYNCHRONIZE( pInstanceDungeonInfo->csInstanceDungeonType );

	const InstanceDungeonType * pInstanceDungeonType = findInstanceDungeonType( pInstanceDungeonInfo, nLayer );

	if( !pInstanceDungeonType )
		return -1;

	THREAD_SYNCHRONIZE1( pInstanceDungeonType->csInstanceDungeonType );

	return pInstanceDungeonType->getAliveRespawnGroupMonsterCount( nRespawnGroup );
}

const unsigned short InstanceDungeonManager::CreateInstanceDungeon( int nInstanceDungeonID, struct StructPlayer * pPlayer, int nType, const unsigned char nDifficulty )
{
	if( GetInstanceDungeonID( pPlayer->GetPos() ) )
		return RESULT_NOT_ACTABLE_IN_INSTANCE_DUNGEON;

	if( pPlayer->GetBattleArenaID() || pPlayer->IsInBattleArena() )
		return RESULT_NOT_ACTABLE_IN_BATTLE_ARENA;

	InstanceDungeonInfo * pInstanceDungeonInfo = findInstanceDungeonInfo( nInstanceDungeonID );
	if( !pInstanceDungeonInfo )
		return RESULT_NOT_ACTABLE;

	const GameContent::INSTANCE_DUNGEON_BASE * pInstanceDungeonBase = pInstanceDungeonInfo->pInstanceDungeonBase;
	unsigned char nInstanceNo = 0;

	{
		// 여기선 몇 번 레이어에 새 인스턴스 던전이 할당될지 미리 예상할 수 없으므로 전체 레이어에 락을 걸어 줌
		ARCADIA_LOCK( pInstanceDungeonInfo->LockWholeDungeonArea() );

		const char * pszScriptOnCreate = NULL;
		const GameContent::INSTANCE_DUNGEON_TYPE_BASE * pInstanceDungeonTypeBase = NULL;
		{
			THREAD_SYNCHRONIZE( pInstanceDungeonInfo->csInstanceDungeonType );

			if( pInstanceDungeonInfo->IsInstanceFull() )
				return RESULT_LIMIT_MAX;

			for( std::vector< const GameContent::INSTANCE_DUNGEON_TYPE_BASE * >::const_iterator itInstanceDungeonTypeBase = pInstanceDungeonBase->vInstanceDungeonTypeBase.begin();
				itInstanceDungeonTypeBase != pInstanceDungeonBase->vInstanceDungeonTypeBase.end();
				++itInstanceDungeonTypeBase )
			{
				if( nType >= 0 && (*itInstanceDungeonTypeBase)->nID != nType )
				{
					continue;
				}

				if( (*itInstanceDungeonTypeBase)->IsProperLevel( pPlayer->GetLevel() ) )
				{
					pInstanceDungeonTypeBase = (*itInstanceDungeonTypeBase);
					break;
				}
			}

			if( !pInstanceDungeonTypeBase )
				return RESULT_NOT_ACTABLE;

			std::map< int, InstanceDungeonType * >::iterator itInstance;
			int nKey = 0;
			if( pPlayer->IsInParty() )
			{
				nKey = pPlayer->GetPartyID();

				// 파티 인던이 우선 순위가 높다, 솔로 인던이 있었다면 사라진다.
				itInstance = pInstanceDungeonInfo->mapInstanceDungeonType.find( -pPlayer->GetPlayerUID() );
				if( itInstance != pInstanceDungeonInfo->mapInstanceDungeonType.end() && itInstance->second->tDestructionTime > GetArTime() )
				{
					itInstance->second->endInstance();

					pInstanceDungeonInfo->vPendedInstanceDungeonTypeToBeDeleted.push_back( itInstance->second );
					pInstanceDungeonInfo->mapInstanceDungeonType.erase( itInstance );
				}

				itInstance = pInstanceDungeonInfo->mapInstanceDungeonType.find( nKey );
			}
			else
			{
				nKey = -pPlayer->GetPlayerUID();

				itInstance = pInstanceDungeonInfo->mapInstanceDungeonType.find( nKey );
			}

			bool bDelete = false;

			InstanceDungeonType * pInstanceDungeonType = NULL;

			if( itInstance != pInstanceDungeonInfo->mapInstanceDungeonType.end() && itInstance->second->tDestructionTime > GetArTime() )
			{
				// 찾았지만 난이도가 다른 경우, 기존의 던전을 삭제한다
				if( (*itInstance).second->nDifficulty != nDifficulty )
					bDelete = true;
				else
					pInstanceDungeonType = itInstance->second;
			}

			if( !pInstanceDungeonType )
			{
				nInstanceNo = pInstanceDungeonInfo->GetNewInstanceNo();
				if( nInstanceNo > MAX_INSTANCE )
					return RESULT_LIMIT_MAX;

				// 인던을 생성하고 초기화를 한다
				// 만약 입장 조건을 만족하지 못 한다면 취소된다
				pInstanceDungeonType = new InstanceDungeonType( pInstanceDungeonInfo, pInstanceDungeonTypeBase, nInstanceNo, nKey, pPlayer->GetAccountID(), pPlayer->GetSID(), pPlayer->GetAccountName(), pPlayer->GetName(), nDifficulty );

				unsigned short nResult = pInstanceDungeonType->beginInstance( pPlayer );
				if( nResult != RESULT_SUCCESS )
				{
					delete pInstanceDungeonType;
					return nResult;
				}

				// 입장에 성공했다면 이전 인던을 제거한다
				if( bDelete )
				{
					itInstance->second->endInstance();

					pInstanceDungeonInfo->vPendedInstanceDungeonTypeToBeDeleted.push_back( itInstance->second );
					pInstanceDungeonInfo->mapInstanceDungeonType.erase( itInstance );
				}

				pInstanceDungeonInfo->mapInstanceNo.insert( std::make_pair( nInstanceNo, pInstanceDungeonType ) );
				pInstanceDungeonInfo->mapInstanceDungeonType.insert( std::make_pair( nKey, pInstanceDungeonType ) );

				// 인스턴스 생성 시 실행될 스크립트 세팅
				pszScriptOnCreate = pInstanceDungeonTypeBase->szScriptOnCreate;
			}
		}

		// 인스턴스 생성 시 실행될 스크립트 호출
		char szScriptBuffer[ 256 ] = { 0, };
		if( pszScriptOnCreate )
		{
			s_strcpy( szScriptBuffer, _countof( szScriptBuffer ), pszScriptOnCreate );

			char szElementBuffer[ 32 ];
			s_sprintf( szElementBuffer, _countof( szElementBuffer ), "%d", (int)nInstanceNo );
			XStringUtil::Replace( szScriptBuffer, _countof( szScriptBuffer ), "#@layer@#", szElementBuffer );

			LUA()->RunString( szScriptBuffer );
			// 인스턴스 생성 시에는 참여 중인 유저가 없는 상태이기 때문에 유저들의 데이터 변동에 대한 로그(LM_INSTANCE_DUNGEON_PLAYER_INFO_CHANGE)를 남길 필요가 없음
		}
		
		LOG::Log11N4S( LM_INSTANCE_DUNGEON_PROCESS, 0, 0, pInstanceDungeonBase->nID, pInstanceDungeonTypeBase->nID, nInstanceNo, 0, 0, 0, 0, 0, 0,
			"", 0, "", 0, szScriptBuffer, LOG::STR_NTS, "INSTANCE_CREATE", LOG::STR_NTS );
	}

//	if( pInstanceDungeonBase )
//	{
//		ARCADIA_LOCK( ArcadiaServer::Instance().LockObjectWithVisibleRange( pPlayer ) );

//		pPlayer->PendWarp( pInstanceDungeonBase->posDungeon.x, pInstanceDungeonBase->posDungeon.y, nInstanceNo );
//		ArcadiaServer::Instance().SetObjectPriority( pPlayer, ArSchedulerObject::UPDATE_PRIORITY_HIGHEST );
//	}

	return RESULT_SUCCESS;
}

const unsigned short InstanceDungeonManager::JoinInstanceDungeon( int nInstanceDungeonID, struct StructPlayer * pPlayer, ArPosition & posEnter, unsigned char & nEnterLayer, bool bStorePosition )
{
	if( pPlayer->GetBattleArenaID() || pPlayer->IsInBattleArena() )
		return RESULT_NOT_ACTABLE_IN_BATTLE_ARENA;

	if( bStorePosition )
	{
		pPlayer->StoreCurrentStatesOnEnterInstanceGame( true );
	}

	InstanceDungeonInfo * pInstanceDungeonInfo = findInstanceDungeonInfo( nInstanceDungeonID );

	if( !pInstanceDungeonInfo )
		return RESULT_NOT_ACTABLE;

	InstanceDungeonType * pInstanceDungeonType = NULL;
	const char * pszScriptOnJoin = NULL;
	unsigned char nLayer = 0;

	{
		THREAD_SYNCHRONIZE( pInstanceDungeonInfo->csInstanceDungeonType );

		// 파티 인던이 우선 순위가 높다
		int nKey = pPlayer->IsInParty() ? pPlayer->GetPartyID() : -pPlayer->GetPlayerUID();
		std::map< int, InstanceDungeonType * >::iterator itInstance = pInstanceDungeonInfo->mapInstanceDungeonType.find( nKey );

		if( itInstance == pInstanceDungeonInfo->mapInstanceDungeonType.end() || ( itInstance->second->tDestructionTime && itInstance->second->tDestructionTime <= GetArTime() ) )
			return RESULT_NOT_ACTABLE;

		pInstanceDungeonType = (*itInstance).second;

		THREAD_SYNCHRONIZE1( pInstanceDungeonType->csInstanceDungeonType );

		unsigned short nErrorCode = pInstanceDungeonType->joinInstance( pPlayer );

		if( nErrorCode != RESULT_SUCCESS )
			return nErrorCode;

		// 인스턴스에 참여 시 실행될 스크립트 세팅
		pszScriptOnJoin = pInstanceDungeonType->pInstanceDungeonTypeBase->szScriptOnJoin;
		nLayer = pInstanceDungeonType->nInstanceNo;

		LOG::Log11N4S( LM_INSTANCE_DUNGEON_JOIN, pPlayer->GetAccountID(), pPlayer->GetSID(), nInstanceDungeonID, pInstanceDungeonType->pInstanceDungeonTypeBase->nID, pInstanceDungeonType->nInstanceNo, nKey, 0, 0, pInstanceDungeonType->nPlayerCount, 0, 0,
			pPlayer->GetAccountName(), LOG::STR_NTS, pPlayer->GetName(), LOG::STR_NTS, "", 0, "", 0 );
	}

	// 인스턴스에 참여 시 실행될 스크립트 실행
	char szScriptBuffer[ 256 ] = { 0, };
	if( pszScriptOnJoin && pszScriptOnJoin[ 0 ] )
	{
		s_strcpy( szScriptBuffer, _countof( szScriptBuffer ), pszScriptOnJoin );

		char szElementBuffer[ 32 ];
		s_sprintf( szElementBuffer, _countof( szElementBuffer ), "%d", (int)nLayer );
		XStringUtil::Replace( szScriptBuffer, _countof( szScriptBuffer ), "#@layer@#", szElementBuffer );

		ARCADIA_LOCK( pInstanceDungeonInfo->LockWholeDungeonArea( nLayer ) );

		PlayerBasicInfoForLog prevPBI( pPlayer );

		LUA()->RunString( szScriptBuffer );

		PlayerBasicInfoForLog currentPBI( pPlayer );
		if( prevPBI != currentPBI )
		{
			LOG::Log11N4S( LM_INSTANCE_DUNGEON_PLAYER_INFO_CHANGE, pPlayer->GetAccountID(), pPlayer->GetPlayerUID(), pInstanceDungeonInfo->pInstanceDungeonBase->nID, pInstanceDungeonType->pInstanceDungeonTypeBase->nID, pInstanceDungeonType->nInstanceNo,
				prevPBI.gold.GetRawData(), currentPBI.gold.GetRawData(), prevPBI.exp, currentPBI.exp, prevPBI.jp, currentPBI.jp,
				pPlayer->GetAccountName(), LOG::STR_NTS, pPlayer->GetName(), LOG::STR_NTS, szScriptBuffer, LOG::STR_NTS, "USER_JOIN", LOG::STR_NTS );
		}
	}

	LOG::Log11N4S( LM_INSTANCE_DUNGEON_PROCESS, 0, 0, pInstanceDungeonInfo->pInstanceDungeonBase->nID, pInstanceDungeonType->pInstanceDungeonTypeBase->nID, pInstanceDungeonType->nInstanceNo, 0, 0, 0, 0, 0, 0,
		"", 0, "", 0, szScriptBuffer, LOG::STR_NTS, "USER_JOIN", LOG::STR_NTS );

	// 함수 결과값으로 돌려줘야 할 입장 대상 위치값 세팅
	nEnterLayer = nLayer;
	// 만일 bStorePosition == false 였다면, 좌표를 바꾸지 않고 입장시키는 경우(Ex. 로그인)이므로 좌표값을 캐릭터 현재 좌표로 복사해 줌.
	if( bStorePosition )
		posEnter = pInstanceDungeonInfo->pInstanceDungeonBase->posDungeon;
	else
		posEnter = pPlayer->GetPos();

	return RESULT_SUCCESS;
}

const unsigned short InstanceDungeonManager::LeaveInstanceDungeon( int nInstanceDungeonID, struct StructPlayer * pPlayer )
{
	int nPlayerInstanceDungeonID = GetInstanceDungeonID( pPlayer->GetPos() );
	if( !nPlayerInstanceDungeonID )
		return RESULT_ACTABLE_IN_ONLY_INSTANCE_DUNGEON;

	assert( nPlayerInstanceDungeonID == nInstanceDungeonID );

	InstanceDungeonInfo * pInstanceDungeonInfo = findInstanceDungeonInfo( nInstanceDungeonID );

	if( !pInstanceDungeonInfo )
		return RESULT_NOT_ACTABLE;

	const char * pszScriptOnLeave = NULL;
	unsigned char nLayer = 0;

	// 방에 남아있던 마지막 유저가 방에서 이탈(leaveInstance)하고 csInstanceDungeonType이 풀리면
	// 스크립트를 실행하기 전에 onProcess에서 해당 방을 endInstance & delete 해 버릴 수 있음.
	// 이를 방지하기 위해 여기서 지역락을 먼저 걸어놓고
	// 이탈 처리에 이어 스크립트 실행까지 완료한 후에 지역 락을 풀어서 방이 실제로 삭제될 수 있도록 풀어 줌.
	// * 동기화 요소를 추가하면 추가할 수록 성능이 떨어지므로 여기는 추후에 방의 상태 변수를 두고
	//   파괴되어도 되는 상태를 leaveInstance에서 세팅하지 말고 스크립트 실행 후에 만들도록 수정하고,
	//   지역락은 스크립트 실행하는 블럭에서만 걸리도록 수정해야 함.
	//   현재 여기서는 레이어 번호를 결정지을 수도 없어서 전체 레이어에 락을 걸어야 하지만,
	//   위와 같이 수정하고 나면 스크립트 실행 시점에 유저가 이탈한 방의 레이어 번호를 알 수 있으므로 레이어를 지정해서 락을 걸어야 함.
	ARCADIA_LOCK( pInstanceDungeonInfo->LockWholeDungeonArea() );

	InstanceDungeonType * pInstanceDungeonType = NULL;
	{
		THREAD_SYNCHRONIZE( pInstanceDungeonInfo->csInstanceDungeonType );

		// 파티 인던이 우선 순위가 높다
		int nKey = pPlayer->IsInParty() ? pPlayer->GetPartyID() : -pPlayer->GetPlayerUID();
		std::map< int, InstanceDungeonType * >::iterator itInstance = pInstanceDungeonInfo->mapInstanceDungeonType.find( nKey );

		if( itInstance == pInstanceDungeonInfo->mapInstanceDungeonType.end() )
		{
			// 강제 입장을 통해 인던에 입장한 경우 멤버 정보가 없어 여기 올 수 있다.
			return RESULT_NOT_ACTABLE;
		}

		pInstanceDungeonType = (*itInstance).second;

		THREAD_SYNCHRONIZE1( pInstanceDungeonType->csInstanceDungeonType );

		unsigned short nErrorCode = pInstanceDungeonType->leaveInstance( pPlayer );

		if( nErrorCode != RESULT_SUCCESS )
			return nErrorCode;

		// 인스턴스에서 이탈 시 실행될 스크립트 세팅
		pszScriptOnLeave = pInstanceDungeonType->pInstanceDungeonTypeBase->szScriptOnLeave;
		nLayer = pInstanceDungeonType->nInstanceNo;

		LOG::Log11N4S( LM_INSTANCE_DUNGEON_LEAVE, pPlayer->GetAccountID(), pPlayer->GetSID(), nInstanceDungeonID, pInstanceDungeonType->pInstanceDungeonTypeBase->nID, pInstanceDungeonType->nInstanceNo, nKey, 0, 0, pInstanceDungeonType->nPlayerCount, 0, 0,
			pPlayer->GetAccountName(), LOG::STR_NTS, pPlayer->GetName(), LOG::STR_NTS, "", 0, "", 0 );
	}

	// 인스턴스에서 이탈 시 실행될 스크립트 실행
	char szScriptBuffer[ 256 ] = { 0, };
	if( pszScriptOnLeave && pszScriptOnLeave[ 0 ] )
	{
		s_strcpy( szScriptBuffer, _countof( szScriptBuffer ), pszScriptOnLeave );

		char szElementBuffer[ 32 ];
		s_sprintf( szElementBuffer, _countof( szElementBuffer ), "%d", (int)nLayer );
		XStringUtil::Replace( szScriptBuffer, _countof( szScriptBuffer ), "#@layer@#", szElementBuffer );

		PlayerBasicInfoForLog prevPBI( pPlayer );

		LUA()->RunString( szScriptBuffer );

		PlayerBasicInfoForLog currentPBI( pPlayer );
		if( prevPBI != currentPBI )
		{
			LOG::Log11N4S( LM_INSTANCE_DUNGEON_PLAYER_INFO_CHANGE, pPlayer->GetAccountID(), pPlayer->GetPlayerUID(), pInstanceDungeonInfo->pInstanceDungeonBase->nID, pInstanceDungeonType->pInstanceDungeonTypeBase->nID, pInstanceDungeonType->nInstanceNo,
				prevPBI.gold.GetRawData(), currentPBI.gold.GetRawData(), prevPBI.exp, currentPBI.exp, prevPBI.jp, currentPBI.jp,
				pPlayer->GetAccountName(), LOG::STR_NTS, pPlayer->GetName(), LOG::STR_NTS, szScriptBuffer, LOG::STR_NTS, "USER_LEAVE", LOG::STR_NTS );
		}
	}

	LOG::Log11N4S( LM_INSTANCE_DUNGEON_PROCESS, 0, 0, pInstanceDungeonInfo->pInstanceDungeonBase->nID, pInstanceDungeonType->pInstanceDungeonTypeBase->nID, pInstanceDungeonType->nInstanceNo, 0, 0, 0, 0, 0, 0,
		"", 0, "", 0, szScriptBuffer, LOG::STR_NTS, "USER_LEAVE", LOG::STR_NTS );


	return RESULT_SUCCESS;
}

const unsigned short InstanceDungeonManager::QuitInstanceDungeon( int nInstanceDungeonID, struct StructPlayer * pPlayer )
{
	int nPlayerInstanceDungeonID = GetInstanceDungeonID( pPlayer->GetPos() );
	if( !nPlayerInstanceDungeonID )
		return RESULT_ACTABLE_IN_ONLY_INSTANCE_DUNGEON;

	assert( nPlayerInstanceDungeonID == nInstanceDungeonID );

	InstanceDungeonInfo * pInstanceDungeonInfo = findInstanceDungeonInfo( nInstanceDungeonID );

	if( !pInstanceDungeonInfo )
		return RESULT_NOT_ACTABLE;

	THREAD_SYNCHRONIZE( pInstanceDungeonInfo->csInstanceDungeonType );

	// 파티 인던이 우선 순위가 높다
	int nKey = pPlayer->IsInParty() ? pPlayer->GetPartyID() : -pPlayer->GetPlayerUID();
	std::map< int, InstanceDungeonType * >::iterator itInstance = pInstanceDungeonInfo->mapInstanceDungeonType.find( nKey );

	if( itInstance == pInstanceDungeonInfo->mapInstanceDungeonType.end() )
	{
		assert( 0 );
		return RESULT_NOT_ACTABLE;
	}

	InstanceDungeonType * pInstanceDungeonType = (*itInstance).second;

	if( pInstanceDungeonType->nKey > 0 )
	{
		int nPartyID = pInstanceDungeonType->nKey;
		QuitFunctor quit;
		PartyManager::GetInstance().DoEachMember( nPartyID, quit );
	}
	else
	{
		ARCADIA_LOCK( ArcadiaServer::Instance().LockObjectWithVisibleRange( pPlayer ) );

		ArPosition pos;
		pPlayer->GetPositionOnEnterInstanceGame( &pos );

		unsigned char nLayer = 0;

		int current_channel = ChannelManager::GetChannelId( pPlayer->GetX(), pPlayer->GetY() );
		int target_channel = ChannelManager::GetChannelId( pos.x, pos.y );

		if( current_channel && current_channel == target_channel )
		{
			nLayer = pPlayer->GetLayer();
		}
		else if( target_channel )
		{
			nLayer = ChannelManager::GetProperLayer( pos.x, pos.y );
		}

		pPlayer->PendWarp( pos.GetX(), pos.GetY(), nLayer );
		ArcadiaServer::Instance().SetObjectPriority( pPlayer, ArSchedulerObject::UPDATE_PRIORITY_HIGHEST );
	}

	return RESULT_SUCCESS;
}

const int InstanceDungeonManager::DoEachPlayerInInstanceDungeon( int nInstanceDungeonID, const unsigned char nLayer, ArObjectFunctor & functor )
{
	InstanceDungeonInfo * pInstanceDungeonInfo = findInstanceDungeonInfo( nInstanceDungeonID );

	if( !pInstanceDungeonInfo )
		return 0;

	THREAD_SYNCHRONIZE( pInstanceDungeonInfo->csInstanceDungeonType );

	InstanceDungeonType * pInstanceDungeonType = findInstanceDungeonType( pInstanceDungeonInfo, nLayer );

	if( !pInstanceDungeonType )
		return 0;

	THREAD_SYNCHRONIZE1( pInstanceDungeonType->csInstanceDungeonType );

	return pInstanceDungeonType->doEachPlayer( functor );
}

void InstanceDungeonManager::PendRespawnMonster( const int nInstanceDungeonID, const AR_UNIT x, const AR_UNIT y, const unsigned char nLayer, const int nMonsterID, const int nRespawnCount, const AR_TIME nLifeTime, const AR_HANDLE hInitialEnemy )
{
	for( std::vector< InstanceDungeonInfo * >::iterator it = m_vInstanceDungeonInfo.begin() ; it != m_vInstanceDungeonInfo.end() ; ++it )
	{
		if( (*it)->pInstanceDungeonBase->nID != nInstanceDungeonID )
			continue;

		// 지역 락을 걸고 호출되는 함수이므로 csInstanceDungeon을 걸면 안됨
		{
			THREAD_SYNCHRONIZE( (*it)->csPendedRespawn );

			(*it)->vPendedRespawn.push_back( new GameContent::PENDED_RESPAWN_INFO( x, y, nLayer, nMonsterID, nRespawnCount, true, nLifeTime, hInitialEnemy ) );
		}

		return;
	}

	assert( 0 );
}

bool InstanceDungeonManager::PendRespawnMonster( const int nRespawnID, const int nInstanceDungeonID, const int nLayer )
{
	InstanceDungeonInfo * pInstanceDungeonInfo = findInstanceDungeonInfo( nInstanceDungeonID );

	if( !pInstanceDungeonInfo )
		return false;

	THREAD_SYNCHRONIZE( pInstanceDungeonInfo->csInstanceDungeonType );

	InstanceDungeonType * pInstanceDungeonType = findInstanceDungeonType( pInstanceDungeonInfo, nLayer );

	if( !pInstanceDungeonType )
		return false;

	THREAD_SYNCHRONIZE1( pInstanceDungeonType->csInstanceDungeonType );
	
	const GameContent::INSTANCE_DUNGEON_MONSTER_RESPAWN_INFO * pRespawnInfo = NULL;
	for( std::vector< const GameContent::INSTANCE_DUNGEON_MONSTER_RESPAWN_INFO * >::const_iterator itRespawn = pInstanceDungeonType->pInstanceDungeonTypeBase->vRespawnInfo.begin() ;
		itRespawn != pInstanceDungeonType->pInstanceDungeonTypeBase->vRespawnInfo.end() ; ++itRespawn )
	{
		if( (*itRespawn)->nID != nRespawnID )
			continue;

		pRespawnInfo = (*itRespawn);
		break;
	}
	
	if( !pRespawnInfo )
		return false;

	pInstanceDungeonType->vPendedMonsterRespawn.push_back( InstanceDungeonType::PendedMonsterRespawnInfo( pRespawnInfo, GetArTime() ) );
	return true;
}

void InstanceDungeonManager::PendRespawnProp( const int nInstanceDungeonID, const int _nPropId, const AR_UNIT _x, const AR_UNIT _y, const unsigned char _layer, const float _fZOffset, const float _fRotateX, const float _fRotateY, const float _fRotateZ, const float _fScaleX, const float _fScaleY, const float _fScaleZ, const bool _bLockHeight, const float _fLockHeight )
{
	for( std::vector< InstanceDungeonInfo * >::iterator it = m_vInstanceDungeonInfo.begin() ; it != m_vInstanceDungeonInfo.end() ; ++it )
	{
		if( (*it)->pInstanceDungeonBase->nID != nInstanceDungeonID )
			continue;

		// 지역 락을 걸고 호출되는 함수이므로 csInstanceDungeon을 걸면 안됨
		{
			THREAD_SYNCHRONIZE( (*it)->csPendedRespawnProp );

			(*it)->vPendedRespawnProp.push_back( new GameContent::FIELD_PROP_RESPAWN_INFO( _nPropId, _x, _y, _layer, _fZOffset, _fRotateX, _fRotateY, _fRotateZ, _fScaleX, _fScaleY, _fScaleZ, _bLockHeight, _fLockHeight ) );
		}

		return;
	}

	assert( 0 );
}

void InstanceDungeonManager::onProcess( int nThreadIdx )
{
	char buf[255];
	s_sprintf( buf, _countof( buf ), "thread.scheduler.%d.proc", nThreadIdx );
	ENV().Set( buf, "InstanceDungeonManager" );

	AR_TIME t = GetArTime();

	s_sprintf( s_ThreadInfo.job_info, _countof( s_ThreadInfo.job_info ), "InstanceDungeonManager(0x%08X)", (UINT_PTR)this );
	s_ThreadInfo.last_execute_time = t;

	if( !IsInitialized() )
		return;

	for( std::vector< InstanceDungeonInfo * >::iterator it = m_vInstanceDungeonInfo.begin() ; it != m_vInstanceDungeonInfo.end() ; ++it )
	{
		InstanceDungeonInfo * pInstanceDungeonInfo = (*it);

		// 모든 생성되어 있는 인스턴스에 대한 처리를 해야 하므로 전체 레이어에 대해 락을 걸어 줌
		ARCADIA_LOCK( pInstanceDungeonInfo->LockWholeDungeonArea() );

		{
			THREAD_SYNCHRONIZE( pInstanceDungeonInfo->csInstanceDungeonType );

			// 삭제 대기 중인 인스턴스 던전 삭제 처리
			// * 현재 생성되어 있는 방들을 처리하기 전에 이걸 먼저 처리함으로써
			//   이번 onProcess에서 삭제 대기 상태가 된 인스턴스를 다음 onProcess에서 삭제하도록 여유 시간을 발생시킴.
			for( std::vector< InstanceDungeonType * >::iterator itInstance = pInstanceDungeonInfo->vPendedInstanceDungeonTypeToBeDeleted.begin() ; itInstance != pInstanceDungeonInfo->vPendedInstanceDungeonTypeToBeDeleted.end() ; ++itInstance )
			{
				// 삭제 전에 락을 한 번 걸었다 풀어서 다른 프로세스가 인스턴스 정보를 사용 중일 가능성을 낮춘 후에 삭제
				// 그래봐야 여기서 거는 락 때문에 기다리고 있는 쓰레드가 생기면 말짱 꽝이지만,
				// 인스턴스 정보가 vPendedInstanceDungeonToBeDelete 리스트에 추가되고 여기까지 도달하기 전에 어지간한 것들은 다 종료되기에 충분할 것으로 가정
				{
					THREAD_SYNCHRONIZE( (*itInstance)->csInstanceDungeonType );
				}

				InstanceDungeonType * pInstance = *itInstance;
				pInstanceDungeonInfo->mapInstanceNo.erase( pInstance->nInstanceNo );
				delete pInstance;
			}
			pInstanceDungeonInfo->vPendedInstanceDungeonTypeToBeDeleted.clear();

			// 현재 생성되어 있는 인스턴스 던전 처리
			for( std::map< int, InstanceDungeonType * >::iterator itInstance = pInstanceDungeonInfo->mapInstanceDungeonType.begin() ; itInstance != pInstanceDungeonInfo->mapInstanceDungeonType.end() ; /* 루프에서 ++itInstance 처리 */ )
			{
				InstanceDungeonType * pInstance = (*itInstance).second;

				THREAD_SYNCHRONIZE( pInstance->csInstanceDungeonType );

				// 아무도 없는 방 제거
				if( itInstance->second->tDestructionTime && pInstance->tDestructionTime <= t )
				{
					pInstance->endInstance();

					itInstance = pInstanceDungeonInfo->mapInstanceDungeonType.erase( itInstance );
					pInstanceDungeonInfo->vPendedInstanceDungeonTypeToBeDeleted.push_back( pInstance );

					continue;
				}

				// 종료랑 관계 없는 방이면 리젠 처리
				pInstance->procMonsterRespawn( t );
				pInstance->procHealingPropRespawn( t );

				++itInstance;
			}
		}

		// 리젠 대기 상태인 몬스터를 리젠 처리
		// 위의 처리가 끝나고 하는 이유는 종료될 방은 종료시키고 나서 해야 쓸데없는 몹 리젠/삭제가 반복되지 않기 때문
		pInstanceDungeonInfo->ProcPendedMonsterRespawn();
		pInstanceDungeonInfo->ProcPendedPropRespawn();
	}
}
