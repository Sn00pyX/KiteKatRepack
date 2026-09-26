
#include <ctime>
#include <algorithm>

#include <network/XIOCPConnection.h>
#include <mmo/ArcadiaServer.h>
#include <toolkit/XConsole.h>
#include <toolkit/XSTLUtil.h>
#include <toolkit/XRandom.h>
#include <toolkit/KHash.h>
#include <toolkit/XStringUtil.h>
#include <logging/FileLog.h>
#include <toolkit/XEnv.h>

#include "LogClient/LogClient.h"
#include "ErrorCode/ErrorCode.h"

#include "StructPlayer.h"
#include "GameAllocator.h"
#include "Constant.h"
#include "StructItem.h"
#include "GameRule.h"
#include "DB_Commands.h"
#include "StructSummon.h" 
#include "StructPet.h"
#include "StructMonster.h"
#include "StructSkill.h"
#include "LuaVM.h"
#include "GameContent.h"
#include "SendMessage.h"
#include "GameProc.h"
#include "GameDBManager.h"
#include "PartyManager.h"
#include "GuildManager.h"
#include "ScriptCommon.h"
#include "ChannelManager.h"
#include "StructWorldLocation.h"
#include "StructNPC.h"
#include "DungeonManager.h"
#include "HuntaholicManager.h"
#include "InstanceDungeonManager.h"
#include "BattleArenaManager.h"
#include "Extern.h"
#include "CompeteManager.h"
#include "ItemBase.h"
#include "GlobalVariableManager.h"
#include "ThreadPlayerHelper.h"
#include "SchedulingPerformanceTracker.h"
#include "..\Community\PartyMatchingManager.h"


extern __declspec( thread ) XSEH::THREAD_INFO s_ThreadInfo;

static XCriticalSection			s_ItemLogLock( "s_ItemLogLock" );
static XCriticalSection			s_AutoLock( "s_AutoLock" );
static XCriticalSection			s_ReturnLobbyConnLock( "s_ReturnLobbyConnLock" );
static std::vector< std::pair< int, XIOCPConnection * > >	s_vReturnLobbyConn;
static volatile LONG			s_nReturnLobbyId = 1;
static KHash< int, hashPr_mod_int >	s_hsAutoAccount;
static XCriticalSection			s_FriendsLock( "s_FriendsLock" );


void itemFileLog( StructItem *pItem, const char *szAction )
{
	return;
	THREAD_SYNCRONIZE( s_ItemLogLock );

	FILE *fp = NULL;
	if( fopen_s( &fp, "item_log.txt", "a+" ) != 0 ) return;
	if( fp )
	{
		fprintf( fp, "%10s : %u %u %I64d (%d)\n", szAction, pItem->GetOwnerUID(), pItem->GetOwnSummonUID(), pItem->GetItemUID(), (int)pItem->IsJoinable() );
		fclose( fp );
	}
}

static XCriticalSection			s_ItemIndexlock;
static ItemUID	s_nItemIndex;

static volatile LONG			s_nSummonSID;
static volatile LONG			s_nPetSID;

static volatile LONG			s_nFarmSID;

static volatile LONG							s_nPlayerCount;
XCriticalSection								g_PlayerListlock;		// ShutdownManager(ScriptMisc.cpp)에서 참조해야 하므로 static 객체가 아닌 전역 객체로 선언
XCriticalSection								g_ConnectionTagLock;
std::vector< StructPlayer* >					g_vPlayerList;			// ShutdownManager(ScriptMisc.cpp)에서 참조해야 하므로 static 객체가 아닌 전역 객체로 선언
static KHash< AR_HANDLE, hashPr_string >		s_hsPlayerList;
static KHash< struct IStreamSocketConnection * , hashPr_string_nocase >	s_hsAccountList;

XCriticalSection							StructPlayer::s_csEventState( "s_csEventState" );
std::vector< StructPlayer::EVENT_STATE >	StructPlayer::s_vEventState;

XCriticalSection StructPlayer::s_csPlayerName( "s_csPlayerName" );
KHash< std::string *, hashPr_mod_basic< PlayerUID > >	StructPlayer::s_hsPlayerName;

static const AR_TIME	INVINCIBLE_DURATION				= 2000;		// 무적 유지시간 20초
static const int		UPPERCASE_NAME_BUFFER_SIZE		= 62;		// 캐릭터 이름 대문자 변환 버퍼 크기 62 Byte - DB에 61바이트 최대로 되어있음

struct _PlayerListComposer : public ArObjectFunctor
{
	_PlayerListComposer( std::vector< StructPlayer * > & _pvPlayerList )
		: pvPlayerList( _pvPlayerList )
	{}

	virtual void operator()( ArObject *pObj ) const
	{
		pvPlayerList.push_back( static_cast< StructPlayer * >( pObj ) );
	}

	std::vector< StructPlayer * > & pvPlayerList;
};

static int MIN( const int & l, const int & r )
{
	return ( l < r ? l : r );
}

static int MAX( const int & l, const int & r )
{
	return ( l > r ? l : r );
}

void PlayerDeleter::onProcess( int nThreadIdx )
{
	char buf[255];
	s_sprintf( buf, _countof( buf ), "thread.scheduler.%d.proc", nThreadIdx );
	ENV().Set( buf, "PlayerDeleter" );

	AR_TIME tCurrent = GetArTime();
	s_sprintf( s_ThreadInfo.job_info, _countof( s_ThreadInfo.job_info ), "PlayerDeleter(0x%08X)", (UINT_PTR)this );
	s_ThreadInfo.last_execute_time = tCurrent;

	THREAD_SYNCRONIZE( g_PlayerListlock );

	for( std::vector< StructPlayer * >::iterator it = g_vPlayerList.begin(); it != g_vPlayerList.end(); ++it )
	{
		// 1. pConnection이 NULL인 경우는 LogoutNow가 호출되었으나 g_vPlayerList에 남은 경우(버그겠지 -_-;)
		// 2. pConnection->IsConnected() == false 인 경우는 접속 종료 대기 상태이거나 연결만 끊기고 캐릭터가 월드에서 없어지지 않은 경우
		//    접속 종료 대기 상태인 경우 여기서 로그아웃 시키면 안 되므로 로그아웃 타이머가 걸려있는지 체크
		if( ( !(*it)->pConnection || !(*it)->pConnection->IsConnected() ) && !(*it)->GetLogoutTimer() )
		{
			// onProcess가 호출될 수 있는 녀석이라면 거기서 로그아웃 처리되도록 로그아웃 타이머 세팅
			// Priority가 UPDATE_PRIORITY_IDLE이 아니라는 것은 LogoutNow가 호출되지 않았거나, 호출되어
			// 처리 중이지만 아직 DeleteObject까지는 처리되지 않은 경우.
			if( (*it)->GetFinalPriority() != UPDATE_PRIORITY_IDLE )
			{
				if( !(*it)->IsLogin() || (*it)->CanLogoutNow() )
					(*it)->SetLogoutTimer( tCurrent );
				else
					(*it)->SetLogoutTimer( tCurrent + GameRule::nLogoutTimer );
			}
			// onProcess가 호출될 수도 없는데 월드에 있다면... -_ - 뭥미?
			// DeleteObject는 호출되었으나 LogoutNow는 처리되지 않은 경우.(절대로 일어나서는 안될 상황)
			// Priority를 다시 올려 주고 로그아웃 처리 유도(DeleteObject가 2번 요청되므로 어차피 서버 다운됨)
			else if( (*it)->IsInWorld() )
			{
				assert( 0 );

				(*it)->SetLogoutTimer( tCurrent );
				ArcadiaServer::Instance().SetObjectPriority( (*it), UPDATE_PRIORITY_NORMAL );
			}
			// 월드에 없고 Priority도 UPDATE_PRIORITY_IDLE 이면 DeleteObject 요청까지는 되었고
			// 그 상태에서 IsDeleteable이 false라면 ArObjectDestroyer에서 삭제시키지 못하고 대기하는
			// 상태. 해당 상태는 정상일 수도 있고(뭔가 처리하고 있으니 삭제 대기하셈의 경우)
			// 영원히 캐릭터 정보가 삭제되지 못하고 남는 경우일 수도 있음(구분 못 함 -_ -;)
			// IsDeleteable이 true라면 잠시 후에 ArObjectDestroyer 삭제시킬 거라고 예상(정상임)

			// 결국 정상이거나 비정상일 수도 정상일 수도 있는 경우이므로 무슨 처리를
			// 해야할 지 말아야 할 지도 모르며, 무슨 처리를 해야 한다 해도 가능한 뭣도 없음 -_-;

			// * 로그아웃 처리가 발생한 지 3분 이상 경과한 상태이며 IsDeleteable이 false라면
			//   DB 처리를 대기하고 있다기보다는 뭔가 문제가 생긴 것으로 간주
			else if( (*it)->GetLogoutTime() + 18000 < tCurrent && !(*it)->IsDeleteable() )
			{
				static StructPlayer * pPlayer = (*it);

				XSEH::InvokeUnhandledException( (LONG)0xC000000DL );
			}
		}
	}
}

void StructPlayer::SetMaxSummonSID( int sid )
{
	s_nSummonSID = sid;
}

void StructPlayer::SetMaxFarmSID( int sid )
{
	s_nFarmSID = sid;
}

void StructPlayer::SetTarget( AR_HANDLE target )
{
	m_hTarget = target;
}

void StructPlayer::Save( bool bOnlyCharacter )
{
	// 로그인 미 완료 캐릭터 저장 시도 무시
	if( !IsLoginComplete() )
		return;

	// 이미 로그아웃 된 후에 1번이라도 저장된 캐릭터면 저장 시도 무시
	if( GetLogoutTime() && GetLogoutTime() < m_nLastSaveTime )
		return;

	// 뭐하러 화면에 다른 로그도 안 보이게 찍어대나 -_-;?
	//_cprint( "Save %s[%s]\n", GetName(), ( bOnlyCharacter ? "Character" : "All" ) );

	m_nLastExpSaveTime = GetArTime();

	if( !bOnlyCharacter )	m_nLastSaveTime = GetArTime();

	DBQuery( new DB_UpdateCharacter( this, bOnlyCharacter ) );

	for( int i = 0; i < 6; ++i )
	{
		StructSummon *pSummon = GetSummonAt( i );
		if( !pSummon ) continue;

		pSummon->DBQuery( new DB_UpdateSummon( pSummon ) );
	}

	if( !bOnlyCharacter )
	{
		// { 인생 뭐 있나. 일단 퀘스트 업데
		struct myQuestFunctor : public StructQuestManager::QuestFunctor 
		{
			myQuestFunctor( struct StructPlayer * pPlayer ) : m_pPlayer( pPlayer ) {}

			bool operator()( struct StructQuest * pQuest )
			{
				if( pQuest->GetTimeLimitType() == QuestBase::TIME_LIMIT_TYPE_PERMANENT && !pQuest->IsNeedUpdateToDB() )
					return true;

				m_pPlayer->DBQuery( new DB_UpdateQuest( m_pPlayer,
					m_pPlayer->GetPlayerUID(),
					pQuest->GetQuestID(),
					pQuest->GetStatus( 0 ),
					pQuest->GetStatus( 1 ),
					pQuest->GetStatus( 2 ),
					pQuest->GetStatus( 3 ),
					pQuest->GetStatus( 4 ),
					pQuest->GetStatus( 5 ),
					pQuest->GetRemainTime(),
					pQuest->GetProgress() ) );

				pQuest->TurnOffDbUpdateFlag();

				return true;
			}

			StructPlayer * m_pPlayer;
		} _foQuest( this );

		DoEachActiveQuest( _foQuest );
		// }

		// { 인생 뭐 있나. 아이템도 업데
		size_t idx;
		for( idx = 0; idx < GetItemCount(); ++idx )
		{
			StructItem *pItem = GetItem( static_cast< unsigned int >( idx ) );

			// 펫이 들어있는 펫 우리일 경우 펫 저장(소유권 외에 업데이트 될 정보가 없으므로 저장 안해도 무방, 소유권 정보는 AddPet/RemovePet에서 저장 함)
			//if( pItem->IsPetCage() && pItem->GetInstanceFlag().IsOn( ItemInstance::ITEM_FLAG_CONTAIN_PET ) && pItem->GetPetStruct() )
			//{
			//	StructPet *pPet = pItem->GetPetStruct();
			//	pPet->DBQuery( new DB_UpdatePet( pPet ) );
			//}

			if( !pItem->IsNeedUpdateToDB() && !pItem->IsExpireItem() ) continue;

			pItem->DBQuery( new DB_UpdateItem( pItem ) );
		}

		for( idx = 0; idx < GetStorageItemCount(); ++idx )
		{
			StructItem *pItem = GetStorageItem( static_cast< unsigned int >( idx ) );

			// 펫이 들어있는 펫 우리일 경우 펫 저장(소유권 외에 업데이트 될 정보가 없으므로 저장 안해도 무방, 소유권 정보는 AddPet/RemovePet에서 저장 함)
			//if( pItem->IsPetCage() && pItem->GetInstanceFlag().IsOn( ItemInstance::ITEM_FLAG_CONTAIN_PET ) && pItem->GetPetStruct() )
			//{
			//	StructPet *pPet = pItem->GetPetStruct();
			//	pPet->DBQuery( new DB_UpdatePet( pPet ) );
			//}

			if( !pItem->IsNeedUpdateToDB() && !pItem->IsExpireItem() ) continue;

			pItem->DBQuery( new DB_UpdateItem( pItem ) );
		}
		// }

		// { 인생 뭐 있을래나-_- 스킬도 업데
		struct mySkillFunctor : public StructCreature::SKILL_POINTER_FUNCTOR
		{
			mySkillFunctor( StructCreature * _pOwner, bool _bSaveAll ) : pOwner( _pOwner ), bSaveAll( _bSaveAll )
			{
				t = GetArTime();
			}

			virtual void onSkill( struct StructSkill* pSkill )
			{
				// 로그아웃 시에는 bSaveAll이 true로 쿨타임이 0초인 경우에도 저장을 한다.
				// 쿨타임이 0초라는 것 자체에도 의미가 있기에 저장이 필요하기는 하나, 이미 DB에 0초로 저장이 되어있다면 굳이 갱신할 필요가 없다.
				// 실제 사용되는 스킬이 몇 안 될 뿐더러 자주 사용되는 스킬들은 대체로 쿨타임이 짧기에 대체로 DB에도 0초로 저장되어있고 로그아웃할 시점에도 0초일 가능성이 높다.
				if( pSkill->GetSkillUID() != StructSkill::SKILL_UID_ITEM_SKILL &&
					pSkill->GetSkillUID() != StructSkill::SKILL_UID_PROP_SKILL &&
					pSkill->GetSkillUID() != StructSkill::SKILL_UID_MONSTER_SKILL &&
					pSkill->GetSkillUID() != StructSkill::SKILL_UID_SUMMON_SKILL &&
					pSkill->GetSkillUID() != StructSkill::SKILL_UID_PET_SKILL &&
					pSkill->GetDBCoolTime() != pSkill->GetRemainCoolTime( t ) &&
					( pSkill->GetRemainCoolTime( t ) > 0 || bSaveAll ) )
				{
					if( pOwner->IsPlayer() )
						static_cast< StructPlayer * >( pOwner )->DBQuery( new DB_UpdateSkill( pOwner, pSkill->GetSkillUID(), pSkill->GetBaseSkillLevel(), pSkill->GetRemainCoolTime( t ) ) );
					else if( pOwner->IsSummon() )
						static_cast< StructSummon * >( pOwner )->DBQuery( new DB_UpdateSkill( pOwner, pSkill->GetSkillUID(), pSkill->GetBaseSkillLevel(), pSkill->GetRemainCoolTime( t ) ) );

					pSkill->SetDBCoolTime( pSkill->GetRemainCoolTime( t ) );
				}
			}

			StructCreature * pOwner;
			AR_TIME t;
			bool bSaveAll;
		} _foSkill( this, !m_bIsLogin );

		EnumActiveSkill( _foSkill );
		
		for( std::vector< StructSummon * >::iterator it = m_vSummonList.begin() ; it != m_vSummonList.end() ; ++it )
		{
			_foSkill.pOwner = (*it);
			(*it)->EnumActiveSkill( _foSkill );
		}

		for( std::vector< StructSummon * >::iterator it = m_vStorageSummonList.begin() ; it != m_vStorageSummonList.end() ; ++it )
		{
			_foSkill.pOwner = (*it);
			(*it)->EnumActiveSkill( _foSkill );
		}
		//}

		// { 인생 뭐 있나. 지속효과도 업데
		struct myStateFunctor : public StructCreature::STATE_FUNCTOR
		{
			myStateFunctor( StructCreature * _pOwner, bool _bSaveAll ) : pOwner( _pOwner ), bSaveAll( _bSaveAll )
			{
				t = GetArTime();
				gap = ENV().GetInt( "game.state_save_period", 3000 );
			}

			virtual void onState( struct StructState* pState )
			{
				// 로그아웃 시에는 모든 지속효과를 저장한다.
				// 값이 저장된 시점으로부터 gap만큼의 시간 변화가 있을 때만 저장을 한다.
				// 저장 주기인 5분으로 인해 유료 지속효과를 제외하고는 그리 높지 않은 빈도로 저장이 되는 것이 의도이나
				// 무분별하게 호출되는 Save를 제어하기가 어려워 저장 주기가 지켜지지 않기 때문에 최소한의 방어책으로 gap을 사용한다.
				// 위의 판단과 같이 Save의 빈도가 높다면 모든 지속효과가 부여되자마자 저장이 되므로 최초 부여 후에 강제로 LastSavedTime을 설정하여 바로 저장을 하지 않는 것 역시 의도이다.

				// 결론적으로 얻을 수 있는 효과는 지속효과가 대체로 짧게 걸리기 때문에 대부분의 지속효과를 DB에 저장하지 않고 엔진에서만 유지 시킬 수 있다.
				// 지속효과의 저장의 빈도를 제어함으로 인해 서버 다운이 발생하였을 때 지속효과가 사라질 수 있으나
				// 일반적으로 gap을 Save 주기보다 짧게 부여하므로 기존과 크게 다르지 않게 동작하며 동일한 문제점을 갖는 것일 뿐이다.
				// 다시 말해 gap을 0초로 설정하여 매 Save 주기마다 저장을 하더라도 근본적으로 지속효과가 사라지는 것은 막을 수 없다.

				// ※	대부분의 지속효과들이 저장을 하기도 전에 사라진다. 높아진 저장 빈도에 의해 이 모든 지속효과가 매번 저장되는 것은 효율적이지 못 하다.
				//		스킬로 부여되는 991개의 지속효과 통계

				//		부여될 지속효과의 지속시간 평균 : 207초
				//		부여될 지속효과의 지속시간 최빈값 : 5초, 지속효과 224개 (22%)
				//		부여될 지속효과의 81.13%는 30초 이내에 사라진다.
				if( !bSaveAll &&
					pState->GetLastSavedTime() + gap > t )
					return;

				pState->Save( pOwner, StructState::USED_IN_DB );
			}

			StructCreature * pOwner;
			AR_TIME t, gap;
			bool bSaveAll;
		} _foState( this, !m_bIsLogin );

		EnumState( _foState );

		for( int i = 0; i < 6; ++i )
		{
			StructSummon *pSummon = GetSummonAt( i );

			if( !pSummon )
				continue;

			if( !pSummon->IsNeedToUpdateState() && m_bIsLogin && bOnlyCharacter )
				continue;

			// 소환수가 존재하지 않거나 소환수가 존재하더라도 시야 거리 밖에 소환되어 있으면 건너뛴다.
			// 지역 락의 보호를 받지 못 하기 때문에 지속효과 목록을 순회하다가 무효화가 발생하여 서버 다운으로 이어질 수 있다.
			if( pSummon->IsInWorld() && pSummon->GetPos().GetDistance( GetPos() ) >= GameRule::VISIBLE_RANGE )
				continue;

			_foState.pOwner = pSummon;
			pSummon->EnumState( _foState );

			pSummon->SetNeedToUpdateState( false );
		}
		// }

		// { 호칭 업데이트
		// { 호칭의 획득과 공개는 즉시 저장을 하기에 이곳에서 처리할 필요가 없지만 빈번한 요청이 올 수 있는 호칭 즐겨찾기에 대한 처리는 이곳에서 한다.
		struct myTitleFunctor : public StructTitleManager::TitleFunctor
		{
			myTitleFunctor( struct StructPlayer * pPlayer ) : m_pPlayer( pPlayer ) {}

			bool operator()( struct StructTitle * pTitle )
			{
				if( pTitle->GetDBInsertFlag() )
				{
					m_pPlayer->DBQuery( new DB_InsertTitle( m_pPlayer,
						pTitle->GetSID(),
						m_pPlayer->GetPlayerUID(),
						pTitle->GetCode(),
						pTitle->GetStatus() ) );
				}
				else if( pTitle->GetDBUpdateFlag() )
				{
					m_pPlayer->DBQuery( new DB_UpdateTitle( m_pPlayer,
						pTitle->GetSID(),
						pTitle->GetStatus() ) );
				}

				pTitle->SetDBInsertFlag( false );
				pTitle->SetDBUpdateFlag( false );

				return true;
			}

			StructPlayer * m_pPlayer;
		} _foTitle( this );

		DoEachTitle( _foTitle );
		// }

		// { 호칭 조건 업데이트
		struct myTitleConditionFunctor : public StructTitleManager::TitleConditionFunctor
		{
			myTitleConditionFunctor( struct StructPlayer * pPlayer ) : m_pPlayer( pPlayer ) {}

			bool operator()( struct StructTitleCondition * pTitleCondition )
			{
				// 저장이 필요하지 않은 호칭 조건
				if( !pTitleCondition->GetSID() )
					return true;

				if( pTitleCondition->GetDBInsertFlag() )
				{
					m_pPlayer->DBQuery( new DB_InsertTitleCondition( m_pPlayer,
						pTitleCondition->GetSID(),
						m_pPlayer->GetPlayerUID(),
						pTitleCondition->GetType(),
						pTitleCondition->GetCount() ) );
				}
				else if( pTitleCondition->GetDBUpdateFlag() )
				{
					m_pPlayer->DBQuery( new DB_UpdateTitleCondition( m_pPlayer,
						pTitleCondition->GetSID(),
						pTitleCondition->GetCount() ) );
				}

				pTitleCondition->SetDBInsertFlag( false );
				pTitleCondition->SetDBUpdateFlag( false );

				return true;
			}

			StructPlayer * m_pPlayer;
		} _foTitleCondition( this );

		DoEachTitleCondition( _foTitleCondition );
		// }
	}
}

int	StructPlayer::AllocSummonSID()
{
	return InterlockedIncrement( &s_nSummonSID );
}

int StructPlayer::AllocFarmSID()
{
	return InterlockedIncrement( &s_nFarmSID );
}

void StructPlayer::SetMaxPetSID( int sid )
{
	s_nPetSID = sid;
}

int StructPlayer::AllocPetSID()
{
	return InterlockedIncrement( &s_nPetSID );
}

void StructPlayer::SetAccount( const int nAccountID, const char *szAccount )
{
	s_strcpy( m_szAccountName, _countof( m_szAccountName ), szAccount );
	m_nAccountID = nAccountID;
}

int StructPlayer::GetAccountID() const
{
	return m_nAccountID;
}

const char * StructPlayer::GetAccountName() const
{
	return m_szAccountName;
}

void StructPlayer::BindProperty()
{
	StructPlayer temp( 0 );
	temp.Bind( "hp", &temp.m_nHP );
	temp.Bind( "mp", &temp.m_nMP );
	temp.Bind( "stamina", &temp.m_nStamina );
	temp.Bind( "max_hp", &temp.m_nMaxHP );
	temp.Bind( "max_mp", &temp.m_nMaxMP );
	temp.Bind( "max_stamina", &temp.m_nMaxStamina );
	temp.Bind( "login", &temp.m_bIsLogin );
	temp.BindCString( "account", temp.m_szAccountName, _countof( temp.m_szAccountName ) );
	temp.BindCString( "name", temp.m_szName, _countof( temp.m_szName ) );
	temp.Bind( "gold", const_cast< __int64 * >( &temp.m_nGold.GetRawData() ) );
	temp.Bind( "exp", &temp.m_nEXP );
	temp.Bind( "jp", &temp.m_nJobPoint );
	temp.Bind( "tp", &temp.m_nTalentPoint );
	temp.Bind( "job_level", &temp.m_nJobLevel );
	temp.Bind( "jlv", &temp.m_nJobLevel );
	temp.Bind( "jlv_0", &temp.m_nPrevJobLevel[0] );
	temp.Bind( "jlv_1", &temp.m_nPrevJobLevel[1] );
	temp.Bind( "jlv_2", &temp.m_nPrevJobLevel[2] );
	temp.Bind( "job_0", &temp.m_nPrevJobId[0] );
	temp.Bind( "job_1", &temp.m_nPrevJobId[1] );
	temp.Bind( "job_2", &temp.m_nPrevJobId[2] );
	temp.Bind( "job", &temp.m_nJob );
	temp.Bind( "level", &temp.m_nLevel );
	temp.Bind( "lv", &temp.m_nLevel );
	temp.Bind( "max_reached_level", &temp.m_nMaxReachedLevel );
	temp.Bind( "str", const_cast< __int64 * >( &temp.m_Stat.strength.get() ) );
	temp.Bind( "agi", const_cast< __int64 * >( &temp.m_Stat.agility.get() ) );
	temp.Bind( "dex", const_cast< __int64 * >( &temp.m_Stat.dexterity.get() ) );
	temp.Bind( "int", const_cast< __int64 * >( &temp.m_Stat.intelligence.get() ) );
	temp.Bind( "luck", const_cast< __int64 * >( &temp.m_Stat.luck.get() ) );
	temp.Bind( "vital", const_cast< __int64 * >( &temp.m_Stat.vital.get() ) );
	temp.Bind( "mental", const_cast< __int64 * >( &temp.m_Stat.mentality.get() ) );
	temp.Bind( "permission", &temp.m_nPermission );
	temp.Bind( "huntaholic_point", &temp.m_nHuntaholicPoint );
	temp.Bind( "huntaholic_ent", &temp.m_nHuntaholicEnterableCount );
	temp.Bind( "ethereal_stone", &temp.m_nEtherealStoneDurability );
	temp.Bind( "pk_count", &temp.m_nPKC );
	temp.Bind( "dk_count", &temp.m_nDKC );
	temp.Bind( "premium", &temp.m_bIsGaiaMember );
	temp.Bind( "prem_remain",		(int*)&temp.m_nGaiaValidTime );
	temp.Bind( "race", &temp.m_nRace );
	temp.Bind( "character_checksum", &temp.m_nCharacterChecksum );
	temp.Bind( "charisma", &temp.m_nCharisma );
	temp.Bind( "immoral", (__int64*)&(temp.m_fImmoralPoint.get()) );
	temp.Bind( "login_count", (int*)&temp.m_nLoginCount );
	temp.Bind( "login_time", (int*)&temp.m_nLoginTime );
	temp.Bind( "sex", &temp.m_nSex );
	temp.Bind( "x", &temp.mv.x );
	temp.Bind( "y", &temp.mv.y );
	temp.Bind( "layer", &temp.layer );
	temp.Bind( "weight", &temp.m_fWeight );	
	temp.Bind( "max_weight", const_cast< __int64 * >( &temp.m_Attribute.fMaxWeight.get() ) );	
	temp.Bind( "job_depth", &temp.m_nJobDepth );		
	temp.Bind( "critical", const_cast< __int64 * >( &temp.m_Attribute.fCritical.get() ) );
	temp.Bind( "critical_power", const_cast< __int64 * >( &temp.m_Attribute.fCriticalPower.get() ) );
	temp.Bind( "attack_point", const_cast< __int64 * >( &temp.m_Attribute.fAttackPointRight.get() ) );
	temp.Bind( "attack_right",		const_cast< __int64 * >( &temp.m_Attribute.fAttackPointRight.get() ) );
	temp.Bind( "attack_left",		const_cast< __int64 * >( &temp.m_Attribute.fAttackPointLeft.get() ) );
	temp.Bind( "defence", const_cast< __int64 * >( &temp.m_Attribute.fDefence.get() ) );
	temp.Bind( "block_defence",		const_cast< __int64 * >( &temp.m_Attribute.fBlockDefence.get() ) );
	temp.Bind( "magic_pt", const_cast< __int64 * >( &temp.m_Attribute.fMagicPoint.get() ) );
	temp.Bind( "magic_def", const_cast< __int64 * >( &temp.m_Attribute.fMagicDefence.get() ) );
	temp.Bind( "accuracy", const_cast< __int64 * >( &temp.m_Attribute.fAccuracyRight.get() ) );
	temp.Bind( "accuracy_left", const_cast< __int64 * >( &temp.m_Attribute.fAccuracyLeft.get() ) );
	temp.Bind( "avoid", const_cast< __int64 * >( &temp.m_Attribute.fAvoid.get() ) );
	temp.Bind( "magic_avoid", const_cast< __int64 * >( &temp.m_Attribute.fMagicAvoid.get() ) );
	temp.Bind( "block_chance", const_cast< __int64 * >( &temp.m_Attribute.fBlockChance.get() ) );
		temp.Bind("speed", const_cast<__int64*>(&temp.m_Attribute.fMoveSpeed.get()));
	temp.Bind( "attack_range", const_cast< __int64 * >( &temp.m_Attribute.fAttackRange.get() ) );
	temp.Bind( "attack_speed", const_cast< __int64 * >( &temp.m_Attribute.fAttackSpeed.get() ) );
	temp.Bind( "perfect_block", const_cast< __int64 * >( &temp.m_Attribute.fPerfectBlock.get() ) );
	temp.Bind( "physical_ignore", const_cast< __int64 * >( &temp.m_Attribute.fPhysicalDefIgnore.get() ) );
	temp.Bind( "physical_ignore_ratio", const_cast< __int64 * >( &temp.m_Attribute.fPhysicalDefIgnoreRatio.get() ) );
	temp.Bind( "magical_ignore", const_cast< __int64 * >( &temp.m_Attribute.fMagicalDefIgnore.get() ) );
	temp.Bind( "magical_ignore_ratio", const_cast< __int64 * >( &temp.m_Attribute.fMagicalDefIgnoreRatio.get() ) );
	temp.Bind( "physical_penetration", const_cast< __int64 * >( &temp.m_Attribute.fPhysicalPenetration.get() ) );
	temp.Bind( "physical_penetration_ratio", const_cast< __int64 * >( &temp.m_Attribute.fPhysicalPenetrationRatio.get() ) );
	temp.Bind( "magical_penetration", const_cast< __int64 * >( &temp.m_Attribute.fMagicalPenetration.get() ) );
	temp.Bind( "magical_penetration_ratio", const_cast< __int64 * >( &temp.m_Attribute.fMagicalPenetrationRatio.get() ) );
	temp.Bind( "storage_gold", const_cast< __int64 * >( &temp.m_nStorageGold.GetRawData() ) );	
	temp.Bind( "guild_id", &temp.m_nGuildId );
	temp.Bind( "prev_guild_id", &temp.m_nPrevGuildId );
	temp.Bind( "guild_perm", (unsigned char *)&temp.m_nGuildPermission );
	temp.Bind( "party_id", &temp.m_nPartyID );
	temp.Bind( "chaos", &temp.m_nChaos );
	temp.Bind( "skin_color", (int *)&temp.m_nSkinColor );
	temp.Bind( "hair_id", &temp.m_nBaseModelId[ 0 ] );
	temp.Bind( "hair_color_idx", &temp.m_nHairColorIndex );
	temp.Bind( "hair_color_rgb", (int *)&temp.m_nHairColorRGB );
	temp.Bind( "hide_equip_flag", (int *)&temp.m_nHideEquipFlag );
	temp.Bind( "arena_id", &temp.m_nArenaID );
	temp.Bind( "arena_instance_no", &temp.m_nArenaInstanceNo );
	temp.Bind( "ap", &temp.m_nArenaPoint );
	temp.BindCString( "alias", temp.m_szAlias, _countof( temp.m_szAlias ) );
	temp.Bind( "max_chaos", &temp.m_nMaxChaos );
	temp.Bind( "auto_user", &temp.m_bAutoUsed );
	temp.Bind( "pcbang_user", (unsigned char *)&temp.m_nPCBangMode );
	temp.Bind( "playtime",			(int *)&temp.m_nContinuousPlayTime );
	temp.Bind( "playtime_limit1",	(int *)&GameRule::nMaxHealthyGameTime );
	temp.Bind( "playtime_limit2",	(int *)&GameRule::nMaxTiredGameTime );
	temp.Bind( "logouttime",		(int *)&temp.m_nContinuousLogoutTime );
	temp.Bind( "event_code", &temp.m_nEventCode );
	temp.Bind( "age", &temp.m_nAge );
	temp.Bind( "stamina_regen", &temp.m_nStaminaRegenRate );
	temp.Bind( "name_changed", (int *)&temp.m_nNameChanged );
}

AR_TIME StructPlayer::GetGaiaMemberRemainTime()
{
	return m_nGaiaValidTime;
}

void StructPlayer::SetGaiaMemberRemainTime( AR_TIME valid_time )
{
	m_bIsGaiaMember = false;
	m_nGaiaValidTime = valid_time;
	if( m_nGaiaValidTime > GetArTime() ) m_bIsGaiaMember = true;
}

void	StructPlayer::SetSetSecrouteFreePass( bool bIsPremiumUser, int nRestSecond )
{
	if( bIsPremiumUser )
	{
		// AddState -> onUpdateState -> BroadcastStateMessage 때문에 락 필요
		ARCADIA_LOCK( ArcadiaServer::Instance().LockObjectWithVisibleRange( this ) );

		// 시크루트 프리패스 남은 유효 기간이 200일 이상이면 200일까지로만 보이도록 제한(재접하면 어차피 다시 보임, 오버플로우로 인한 오류 방지)
		if( nRestSecond < 0 || nRestSecond >= GameRule::MAX_SECROUTE_REMAIN_TIME )
		{
			nRestSecond = GameRule::MAX_SECROUTE_REMAIN_TIME;
		}

		AR_TIME nNowTime = GetArTime();
		SetGaiaMemberRemainTime( nNowTime + nRestSecond * 100 );
		AddState( StructState::GAIA_MEMBER_SHIP, 0, 1, nNowTime, nNowTime + nRestSecond * 100 );

		// 시크루트 귀환권은 1개만 남겨두고 모두 제거
		StructItem* pItem = NULL;
		__int64 nCount = GetItemCount( (ItemBase::ItemCode) ItemBase::ITEM_CODE_SECROUTE_WARP_TICKET );
		while( nCount > 1 )
		{
			pItem = FindItem( (ItemBase::ItemCode) ItemBase::ITEM_CODE_SECROUTE_WARP_TICKET );
			nCount -= pItem->GetCount();
			EraseItem( pItem, 1 );
		}

		// 다 없어져 버렸으면 지급
		if( !nCount )
		{
			pItem = StructItem::AllocItem( 0, ItemBase::ITEM_CODE_SECROUTE_WARP_TICKET, 1, ItemInstance::BY_MARKET, -1, -1, -1, 0, 0, 0, 0, 0, 0, time( NULL ) + nRestSecond );
			pItem->SetIdx( 0 );		// 새로 지급되는 아이템이니 새로운 Idx를 받아서 쓰도록 함
			PushItem( pItem, pItem->GetCount() );
		}
		// 남은게 있으면 현재 남은 시크루트 프리패스 시간으로 재설정
		else
		{
			pItem = FindItem( (ItemBase::ItemCode) ItemBase::ITEM_CODE_SECROUTE_WARP_TICKET );
			pItem->SetRemainTime( time( NULL ) + nRestSecond );
			SendItemMessage( this, pItem );
		}
	}
	else
	{
		// 시크루트 귀환권 모두 제거
		StructItem * pItem = NULL;
		__int64 nCount = GetItemCount( (ItemBase::ItemCode) ItemBase::ITEM_CODE_SECROUTE_WARP_TICKET );
		while( nCount > 0 )
		{
			pItem = FindItem( (ItemBase::ItemCode) ItemBase::ITEM_CODE_SECROUTE_WARP_TICKET );
			nCount -= pItem->GetCount();
			EraseItem( pItem, 1 );
		}
	}
}

// 인 게임 내에서 특정 계정에 따라 권한을 주는 처리는 가급적 여기서 한다
void StructPlayer::SetAccountAuthority( int AuthorityType, int Duration )
{
	// Duration == 0: 그 권한을 해제
	switch( AuthorityType )
	{
	case GameRule::AUTHORITY_SECROUTE:
		{
			SetSetSecrouteFreePass( ( Duration != 0 ), Duration );
		}
		break;

	default:
		break;
	}
}

void StructPlayer::onChangeProperty( const std::string & strKey, const char *data )
{
	const char *szKey = strKey.c_str();

	if( !_stricmp( szKey, "hp" ) )
	{
		BroadcastHPMPMsg( this, atoi( data ), 0 );

		return;
	}
	else if( !_stricmp( szKey, "mp" ) )
	{
		BroadcastHPMPMsg( this, 0, atoi( data ) );

		return;
	}
	else if( !_stricmp( szKey, "lv" ) || !_stricmp( szKey, "level" ) )
	{
		SetEXP( GameContent::GetNeedExp( atoi(data) ) );

		return;
	}
	else if( !_stricmp( szKey, "exp" ) )
	{
		onExpChange();

		return;
	}
	else if( !_stricmp( szKey, "gold" ) )
	{
		SendGoldChaosUpdateMsg( this );

		m_TitleManager.UpdateTitleConditionByGold( GetGold() );

		return;
	}
	else if( !_stricmp( szKey, "chaos" ) )
	{
		m_QuestManager.UpdateQuestStatusByParameter( QuestBase::QUEST_PARAMETER_CHAOS, GetChaos() );
	}
	else if( !_stricmp( szKey, "jlv" ) || !_stricmp( szKey, "job_level" ))	
	{
		m_nJobDepth = GetJobDepth();
		m_QuestManager.UpdateQuestStatusByJobLevel( GetJobDepth(), GetJobLevel() );
		CalculateStat();
	}
	else if( !_stricmp( szKey, "immoral" ) )
	{
		BroadcastStatusMessage( this );

		m_TitleManager.UpdateTitleConditionByImmoralPoint( GetImmoralPoint() );
	}
	else if( !_stricmp( szKey, "hair_color_idx" ) || !_stricmp( szKey, "hair_color_rgb" ) || !_stricmp( szKey, "hair_id" ) )
	{
		BroadcastHairInfo( this );
		DBQuery( new DB_UpdateCharacterHair( this ) );
	}
	// 직업이 바뀌었으면 스탯 재계산
	else if( !_stricmp( szKey, "job" ) )	
	{
		onBeforeResetJob();

		m_nJobDepth = GetJobDepth();

		// 로그를 위한 TP 증가량
		int nTP = GetTalentPoint();
		onAfterResetJob();
		nTP = GetTalentPoint() - nTP;

		CalculateStat();

		LOG::Log11N4S( LM_CHARACTER_CHANGE_JOB, GetAccountID(), GetSID(), 0, GetPrevJobId( m_nJobDepth - 1 ), GetJobId(), GetPrevJobLevel( m_nJobDepth - 1 ), GetJobLevel(), nTP, GetTalentPoint(), 0, 0, GetAccountName(), LOG::STR_NTS, GetName(), LOG::STR_NTS, "", 0, "", 0 );
	}
	else if( !_stricmp( szKey, "skin_color" ) )
	{
		BroadcastPropertyMessage( GetRX(), GetRY(), GetLayer(), GetHandle(), "skin_color", static_cast< int >( GetSkinColor() ) );
		DBQuery( new DB_UpdateCharacterSkin( this ) );
	}
	else if( !_stricmp( szKey, "sex" ) )
	{
		BroadcastPropertyMessage( GetRX(), GetRY(), GetLayer(), GetHandle(), "sex", static_cast< int >( GetSex() ) );
		DBQuery( new DB_UpdateCharacterSex( this ) );
	}

	SendPropertyMessage( this, GetHandle(), szKey, GetAsString( szKey ).c_str() );
}

void StructPlayer::onBeforeChangeProperty( const std::string & strKey, const char *data )
{
	const char *szKey = strKey.c_str();

	if( !_stricmp( szKey, "tp" ) )
	{
		LOG::Log11N4S( LM_CHARACTER_GAIN_TP, GetAccountID(), 
			GetSID(), 0, 
			GetPrevJobId( m_nJobDepth - 1 ), GetJobId(),
			GetPrevJobLevel( m_nJobDepth - 1 ), 	GetJobLevel(),
			GetTalentPoint(), atoi( data ), 
			0, 0,
			GetAccountName(), LOG::STR_NTS, 
			GetName(), LOG::STR_NTS,
			"", 0,
			"ByScript", LOG::STR_NTS );

		return;
	}
}

void StructPlayer::EraseReturnLobbyConnection( int conn_id )
{
	THREAD_SYNCRONIZE( s_ReturnLobbyConnLock );

	for( std::vector< std::pair< int, XIOCPConnection * > >::iterator it = s_vReturnLobbyConn.begin(); it != s_vReturnLobbyConn.end(); ++it )
	{
		if( (*it).first == conn_id )
		{
			s_vReturnLobbyConn.erase( it );
			break;
		}
	}
}

int StructPlayer::SetReturnLobbyConnection( class XIOCPConnection * pConn )
{
	THREAD_SYNCRONIZE( s_ReturnLobbyConnLock );

	InterlockedIncrement( &s_nReturnLobbyId );

	s_vReturnLobbyConn.push_back( std::pair<int, class XIOCPConnection *>( static_cast< int >( s_nReturnLobbyId ), pConn ) );

	m_nReturnLobbyConnId = s_nReturnLobbyId;

	return m_nReturnLobbyConnId;
}

void StructPlayer::EraseFromAutoAccount( int account_id )
{
	THREAD_SYNCRONIZE( s_AutoLock );

	s_hsAutoAccount.erase( account_id );
}

void StructPlayer::AddToAutoAccountList( int account_id )
{
	THREAD_SYNCRONIZE( s_AutoLock );

	s_hsAutoAccount.add( account_id, account_id );
}

void StructPlayer::ClearAutoAccountList()
{
	THREAD_SYNCRONIZE( s_AutoLock );

	s_hsAutoAccount.clear();
}

bool StructPlayer::IsAutoAccount( int account_id )
{
	THREAD_SYNCRONIZE( s_AutoLock );

	if( s_hsAutoAccount.has( account_id ) )
		return true;

	return false;
}

StructPlayer* StructPlayer::AllocPlayer()
{
	struct _myIntializer : GameAllocateFunctor
	{
		virtual void operator()( void * pObj, AR_HANDLE handle )
		{
			new (pObj) StructPlayer( handle );
		}
	};

	StructPlayer* p;

	AR_HANDLE handle = allocPlayerStruct( &p, _myIntializer() );

	//new (p) StructPlayer( handle );

	return p;
}

StructPlayer::~StructPlayer()
{	
	if( bIsDeleted ) assert( 0 );

	// 부스 구경 그만~
	if( m_pBoothOpener )
	{
		THREAD_SYNCRONIZE( StructPlayer::GetBoothLock() );

		StopWatchBooth();
	}

	// 아이템 제거(소환수 카드와 펫 우리에 테이밍 된 소환수/펫 정보는 아이템 삭제에서 함께 처리)
	{
		// 인벤토리
		StructItem *pItem;
		size_t idx;
		ItemUID prevUID = 0;
		for( idx = 0; idx < m_Inventory.GetCount() ; ++idx )
		{
			pItem = m_Inventory.Get( idx );

			itemFileLog( pItem, "DEL" );

			if( pItem->GetOwnerHandle() != GetHandle() || pItem->GetIdx() != idx || pItem->GetPreviousUID() != prevUID )
			{
				_cprint( "wrong indexed item in inventory detected. Account[%s] Character[%s] Item(%I64d)\n", GetAccountName(), GetName(), pItem->GetItemUID() );
				FILELOG( "wrong indexed item in inventory detected. Account[%s] Character[%s] Item(%I64d)", GetAccountName(), GetName(), pItem->GetItemUID() );
				assert( 0 );
			}

			prevUID = pItem->GetItemUID();

			// _oprint( "5:" );
			StructItem::PendFreeItem( pItem );
		}

		// 창고
		prevUID = 0;
		for( idx = 0; idx < m_Storage.GetCount(); ++idx )
		{
			pItem = m_Storage.Get( idx );

			itemFileLog( pItem, "DEL" );

			if( pItem->GetOwnerHandle() != GetHandle() || pItem->GetIdx() != idx || pItem->GetPreviousUID() != prevUID )
			{
				_cprint( "wrong indexed item in storage detected. Account[%s] Character[%s] Item(%I64d)\n", GetAccountName(), GetName(), pItem->GetItemUID() );
				FILELOG( "wrong indexed item in storage detected. Account[%s] Character[%s] Item(%I64d)", GetAccountName(), GetName(), pItem->GetItemUID() );
				assert( 0 );
			}
			prevUID = pItem->GetItemUID();

			// _oprint( "5:" );
			StructItem::PendFreeItem( pItem );
		}

		// 농장
		for( idx = 0; idx < GameRule::FARM_MAX_COUNT; ++idx )
		{
			if( !m_vFarmedSummonInfo[idx] ) continue;
			pItem = m_vFarmedSummonInfo[idx]->item;

			itemFileLog( pItem, "DEL" );

			// 농장에 맡겨진 카드는 Index를 가지지 않는다.
			if( pItem->GetOwnerHandle() != GetHandle() || pItem->GetIdx() != 0 )
			{
				assert( 0 );
			}

			// _oprint( "5:" );
			StructItem::PendFreeItem( pItem );
			delete m_vFarmedSummonInfo[idx];
		}
	}
	m_vSummonList.clear();
	m_vStorageSummonList.clear();
	m_vPetList.clear();
	m_vStoragePetList.clear();

	InterlockedDecrement( &s_nPlayerCount );

	if( m_bHasLogined )
	{
		THREAD_SYNCRONIZE( g_PlayerListlock );


		g_vPlayerList.back()->m_nPlayerListIndex = m_nPlayerListIndex;
		vector_fast_erase( &g_vPlayerList, g_vPlayerList.begin() + m_nPlayerListIndex );

		char szUpperName[UPPERCASE_NAME_BUFFER_SIZE];
		XStringUtil::ToUpperThroughWChar( szUpperName, _countof( szUpperName ), GetName(), strlen( GetName() ), ENV().GetInt( "CodePage", CP_ACP ) );
		s_hsPlayerList.erase( szUpperName );
	}

	if( m_nReturnLobbyConnId )
	{
		THREAD_SYNCRONIZE( s_ReturnLobbyConnLock );

		for( std::vector< std::pair< int, XIOCPConnection * > >::iterator it = s_vReturnLobbyConn.begin(); it != s_vReturnLobbyConn.end(); ++it )
		{
			if( (*it).first == m_nReturnLobbyConnId )
			{
				reinterpret_cast< _CONNECTION_TAG * >( (*it).second->GetTag() )->nConnId = 0;

				SendResult( (*it).second, TM_CS_RETURN_LOBBY, RESULT_SUCCESS, 0 );

				s_vReturnLobbyConn.erase( it );

				break;
			}
		}
	}

#ifdef _MEM_USAGE_DEBUG
	XSEH::DecreaseAllocCount( "StructPlayer" );
#endif
}

volatile LONG & StructPlayer::GetPlayerCount()
{
	return s_nPlayerCount;
}

const char * StructPlayer::GetPlayerName( const PlayerUID nPlayerUID )
{
	if( !nPlayerUID )
		return "";

	THREAD_SYNCHRONIZE( s_csPlayerName );

	std::string * pName = NULL;
	if( s_hsPlayerName.lookup( nPlayerUID, pName ) )
		return pName->c_str();

	assert( 0 );
	return "";
}

void StructPlayer::RegisterPlayerName( const PlayerUID nPlayerUID, const char * pszName )
{
	THREAD_SYNCHRONIZE( s_csPlayerName );

	std::string * pName = NULL;
	if( !s_hsPlayerName.lookup( nPlayerUID, pName ) )
	{
		pName = new std::string( pszName );
		s_hsPlayerName.add( nPlayerUID, pName );
	}
	else
		*pName = pszName;
}

void StructPlayer::ClearPlayerName()
{
	THREAD_SYNCHRONIZE( s_csPlayerName );

	std::string *pName = NULL;
	bool bQuitFlag = s_hsPlayerName.get_first_value( pName );
	while( bQuitFlag )
	{
		delete pName;
		bQuitFlag = s_hsPlayerName.get_next_value( pName );
	}
	s_hsPlayerName.clear();
}

bool StructPlayer::ProcDelete()
{
	// _oprint( "DEL PLAYER : %08X\n", this );

	prepareFreePlayerStruct( this );

	StructPlayer::~StructPlayer();

	freePlayerStruct( this );

	// _oprint( "END PLAYER : %08X\n", this );

	return true;
}

StructPlayer::StructPlayer( AR_HANDLE handle )
	: m_csPet( "StructPlayer::m_csPet" )
	, m_csHash( "StructPlayer::m_csHash" )
	, m_csHashAccount ( "StructPlayer::m_csHashAccount" )
{
	AR_TIME tCurrent = GetArTime();
	m_nLoginTime = tCurrent;

	m_nLastInvenArrangedTime = 0;
	m_nLastStorageArrangedTime = 0;

	m_bIsSummonable = true;
	m_bIsInfiniteSummonTime = false;

	m_nGaiaValidTime = 0;
	m_bIsGaiaMember = false;

	m_nPCBangMode = 0;

	m_nEventCode = 0;

	// 플레이 타임 포인트 & 시간제 이벤트 관련
	m_nPlayTime = 0;
	m_nLastPlayTimeUpdateTime = tCurrent;
	m_nLastTimeBasedEventTimeScript = tCurrent;
	m_nLastTimeBasedEventTimeDB = tCurrent;
	m_nTotalPlayTime = 0;

	// 경매 관련
	m_nNextAuctionUsableTime = tCurrent;

	m_nAge = 0;

	m_nReturnLobbyConnId = 0;
	m_nRideModifier = 0;
	m_nSpeedModifier = 0;
	m_nMaxWeightModifier = 0;
	m_nLastAcceptQuest = 0;
	m_nLastProcessTime = 0;
	m_nWorldLocationId = 0;
	m_pWorldLocation = NULL;

	m_bNonNPCDialog = false;
	m_nInvalidScriptTriggerCount = 0;

	m_nStamina = 0;
	m_nMaxStamina = 0;
	m_nStaminaRegenRate = 0;
	m_nStaminaRegenBonus = 0;
	m_nLogoutDuration = 0;

	m_nMaxChaos = 0;
	m_nChaos = 0;

	m_pBoothOpener = NULL;
	m_hTarget = 0;
	m_nCompeteID = 0;

	m_nArenaID = 0;
	m_nArenaInstanceNo = 0;
	m_tArenaBlock = 0;
	m_nArenaPenaltyCount = 0;
	m_tArenaPenaltyDecrease = 0;
	m_nArenaPoint = 0;
	m_nArenaMVPCount = 0;
	memset( m_anArenaRecord, 0, sizeof( m_anArenaRecord ) );

	m_bUseAlias = false;

	m_hHandle = handle;
	m_nUID = 0;

	m_nMaxHP = 120000;
	m_nHP = 120000;

	m_nArObjectType = ArObject::CLIENT_OBJECT;

	m_szName[0] = 0;
	m_szAlias[0] = 0;
	m_nCharacterChecksum = 0;

	m_nPartyID = 0;
	m_nPrevPartyBroadcastedHP = 0;
	m_nPrevPartyBroadcastedMP = 0;

	m_nSex = 0;
	m_nCharisma = 0;

	clearTradeInfo();
	m_bDeadDialogProc = false;
	m_bIsLogin = false;
	m_bHasLogined = false;
	m_bIsSitDown = false;
	m_bIsBattleMode = false;
	m_bStaminaSave = false;
	m_bSuperSave = false;
	m_eSummonStaminaSaveType = SUMMON_STAMINA_SAVE_NONE;
	m_bWalk = false;
	m_bIsStorageLoaded = false;
	m_bIsStorageRequested = false;
	m_nStorageGold.SetRawData( 0 );
	m_nStorageGoldItemID = 0;
	m_Inventory.SetEventReceiver( this );
	m_Storage.SetEventReceiver( this );
	m_nIsUsingStorage = CLOSE_STORAGE;
	m_BoothStatus = IS_NOT_BOOTH;

	m_nSkinColor = 0;
	m_nLoginCount = 0;
	m_nLogoutTime = 0;
	m_nLastCantAttackTime = 0;
	m_nLastSaveTime = tCurrent;
	m_nLastExpSaveTime = tCurrent;
	m_nLogoutTimer = 0;

	m_nContinuousPlayTime = 0;
	m_nContinuousLogoutTime = 0;
	m_nLastContinuousPlayTimeProcTime = 0;

	m_nTradeGold.SetRawData( 0 );
	m_nGold.SetRawData( 0 );
	m_fWeight = 0.0f;

	m_nPlayerListIndex = -1;

	m_nPermission = 0;

	m_nHuntaholicPoint = 0;
	m_nHuntaholicEnterableCount = 0;
	m_tNextHuntaholicEnterableCountRefill = 0;

	m_nJobDepth = 0;

	m_pMainSummon = NULL;
	m_pSubSummon = NULL;
	m_nDoubleSummonTime = GameRule::DEFAULT_DOUBLE_SUMMON_TIME;
	m_nNextUnSummonTime = 0;
	m_nNameChangeTarget = 0;
	m_nPendingUnSummon = 0;

	// 펫 관련
	m_pSummonedPet = NULL;
	m_vPendingUnSummonPetList.reserve( 2 );

	// 호칭 관련
	m_pMainTitle = 0;
	memset( m_pSubTitle, 0, sizeof( m_pSubTitle ) );
	m_tRemainTitleTime = 0;

	m_nPendWarpX = m_nPendWarpY = -1;
	m_nPendWarpLayer = 0;
	m_nPendWarpInstanceDungeonType = -1;

	memset( m_nBaseModelId, 0, sizeof( m_nBaseModelId ) );
	m_nFaceTextureId = 0;
	m_nHairColorIndex = 0;
	m_nHairColorRGB = 0;
	m_nHideEquipFlag = 0;
	memset( m_aBindSummonCard, 0, sizeof( m_aBindSummonCard ) );
	memset( m_aBeltSlotCard, 0, sizeof( m_aBeltSlotCard ) );
	memset( m_vFarmedSummonInfo, 0, sizeof( m_vFarmedSummonInfo ) );

	m_fImmoralPoint = 0;
	m_nPKC = 0;
	m_nDKC = 0;
	m_bIsPK = false;
	m_nTurnOnPkModeTime = 0;
	m_nTurnOffPkModeTime = 0;

	pConnection = NULL;
	m_hTradeTarget = 0;

	m_QuestManager.SetOwner( this );
	m_TitleManager.SetOwner( this );

	m_nLastBindSummonUpdateTime = tCurrent;
	m_nLastStaminaUpdateTime = tCurrent;
	m_nWarpEndTime = tCurrent;

	m_nRidingStateCode = static_cast< StructState::StateCode >( 0 );
	m_nRideIdx = MOUNT_NOTHING;
	m_bUseFasterSpeedInRiding = false;

	memset( m_nItemCoolTime, 0, sizeof( m_nItemCoolTime ) );

	InterlockedIncrement( &s_nPlayerCount );

	m_nLastGlobalChatTime = tCurrent;
	m_nNextChatPenaltyDecreaseTime = tCurrent;

	m_nChatPenalty = 0;
	m_nAdvChatCount = 0;
	m_nNameChanged = 0;
	m_bAutoUsed = false;
	m_bMoveReq = false;
	m_bWarpEnded = false;
	m_nRegionUpdateNeedCount = 0;
	m_nLastAdvChatTime = time( NULL );

	m_hTamingTarget = 0;

	clearPendingBonusMsg();

	m_nLastReadSec = 0;
	m_nTotalCount = 0;
	pFreq[0] = 0;
	pFreq[1] = 0;
	pFreq[2] = 0;
	pFreq[3] = 0;
	m_nLastMin = -1;

#ifdef _MEM_USAGE_DEBUG
	XSEH::IncreaseAllocCount( "StructPlayer" );
#endif
}

bool StructPlayer::ChangeName( const char * szNewName, const bool bRemoveFromFriendAndDenial, const bool bIgnoreNameChangeCount, const AR_HANDLE hItem )
{
	// 이름 미지정(szNewName가 NULL이 되는 경우는 없지만...) 또는 길이 부적절
	if( !szNewName || strlen( szNewName ) < 4 || strlen( szNewName ) > 18 )
	{
		SendChatMessage( false, CHAT_NOTICE, "@NOTICE", this, "@128" );

		return false;
	}

	int code_page = ENV().GetInt( "CodePage", CP_ACP );
	if( !GameRule::IsValidName( code_page, szNewName, static_cast<int>( strlen( szNewName ) ) + 1, 4, 18 ) ||
		GameContent::IsBannedWord( code_page, szNewName ) )
	{
		SendChatMessage( false, CHAT_NOTICE, "@NOTICE", this, "@128" );

		return false;
	}

	// 같은 이름 사용 불가
	if( !strcmp( szNewName, GetName() ) )
	{
		SendChatMessage( false, CHAT_NOTICE, "@NOTICE", this, "@118" );

		return false;
	}

	// 대/소문자 룰에 어긋난 이름 사용 불가
	char szReformattedName[ 20 ];
	s_strcpy( szReformattedName, _countof( szReformattedName ), szNewName );
	if( !GameRule::ReformatName( szReformattedName ) )
	{
		SendChatMessage( false, CHAT_NOTICE, "@NOTICE", this, "@128" );

		return false;
	}

	if( strcmp( szReformattedName, szNewName ) != 0 )
	{
		char szBuf[ 44 ];
		s_sprintf( szBuf, _countof( szBuf ), "@734\v#@correct_name@#\v%s", szReformattedName );
		SendChatMessage( false, CHAT_NOTICE, "@NOTICE", this, szBuf );

		return false;
	}

	if( !bIgnoreNameChangeCount && (InterlockedIncrement( &m_nNameChanged ) > 1) )
	{
		InterlockedDecrement( &m_nNameChanged );

		SendChatMessage( false, CHAT_NOTICE, "@NOTICE", this, "@130" );

		return false;
	}

	DBQuery( new DB_ChangeCharacterName( szNewName, this, bRemoveFromFriendAndDenial, bIgnoreNameChangeCount, hItem ) );

	return true;
}

void StructPlayer::SetMaxItemUID( ItemUID id )
{
	static bool bFlag;

	if( bFlag )
	{
		throw XException( "왜 SetMaxItemUID() 가 두번 불리는 거냐!!" );
	}

	s_nItemIndex = id;
	bFlag = true;
}

ItemUID StructPlayer::allocItemUID()
{	
	THREAD_SYNCRONIZE( &s_ItemIndexlock );
	return ++s_nItemIndex;
}

ItemUID StructPlayer::GetMaxItemUID()
{
	THREAD_SYNCRONIZE( &s_ItemIndexlock );
	return s_nItemIndex;
}

void StructPlayer::OnChangeName( const char *szName, const bool bRemoveFromFriendAndDenial, const bool bIgnoreNameChangeCount, const bool bSuccess )
{
	if( !bSuccess )
	{
		if( !bIgnoreNameChangeCount )
			InterlockedDecrement( &m_nNameChanged );

		return;
	}

	{
		// 이 블록 안에서 사용된 SendFriendsList, SendDenialsList 함수는 s_FriendsLock을 중첩해서 걸었다가 해제하게 됨.
		THREAD_SYNCHRONIZE( s_FriendsLock );

		// 차단 목록을 먼저 갱신하고 친구 목록을 처리해야 차단히 해제되었을 때 친구 목록에서 정상적으로 나타남
		// (나를 차단한 유저의 차단 목록에서 내가 삭제될 시 온라인으로 변경하는 처리에서 SendStatusMessageToFriendOfPlayer 함수가 StructPlayer * 를 받음)
		for( std::vector< std::string >::iterator it = m_vDenial.begin() ; it != m_vDenial.end() ; ++it )
		{
			AR_HANDLE handle = StructPlayer::FindPlayer( (*it).c_str() );
			StructPlayer::iterator pit = StructPlayer::get( handle );
			StructPlayer *pDenial = *pit;

			// 온라인인 차단자는 직접 변경해주고, 나머지는 DB 명령어에서 차단 목록 일괄 변경
			if( pDenial )
			{
				// 내가 차단했으니까 그 사람이 온라인이면 그 사람의 DenialOf 리스트에 이름을 바꿔줘야 함
				std::vector< std::string >::iterator itDenialOf = std::find( pDenial->m_vDenialOf.begin(), pDenial->m_vDenialOf.end(), GetName() );

				if( itDenialOf != pDenial->m_vDenialOf.end() )
				{
					(*itDenialOf) = szName;
				}
			}
		}

		for( std::vector< std::string >::iterator it = m_vDenialOf.begin() ; it != m_vDenialOf.end() ; ++it )
		{
			AR_HANDLE handle = StructPlayer::FindPlayer( (*it).c_str() );
			StructPlayer::iterator pit = StructPlayer::get( handle );
			StructPlayer *pDenial = *pit;

			// 온라인인 차단자는 직접 변경해주고, 나머지는 DB 명령어에서 차단 목록 일괄 변경
			if( pDenial )
			{
				// 나를 차단했으니까 그 사람이 온라인이면 그 사람의 Denial 리스트에 이름을 바꿔줘야 함
				std::vector< std::string >::iterator itDenial = std::find( pDenial->m_vDenial.begin(), pDenial->m_vDenial.end(), GetName() );

				if( itDenial != pDenial->m_vDenial.end() )
				{
					// 자신을 상대방의 차단 목록에서 삭제해야 하는 경우에는 삭제, 아니면 새로운 이름으로 갱신
					if( bRemoveFromFriendAndDenial )
					{
						pDenial->m_vDenial.erase( itDenial );

						// 내가 상대방을 친구로 등록했던 경우 내 친구 목록에 온라인으로 변경
						if( std::find( m_vFriend.begin(), m_vFriend.end(), (*it) ) != m_vFriend.end() )
						{
							SendStatusMessageToFriendOfPlayer( pDenial, this, true );
						}
					}
					else
						(*itDenial) = szName;

					SendDenialsList( pDenial );
				}
			}
		}

		for( std::vector< std::string >::iterator it = m_vFriend.begin() ; it != m_vFriend.end() ; ++it )
		{
			AR_HANDLE handle = StructPlayer::FindPlayer( (*it).c_str() );
			StructPlayer::iterator pit = StructPlayer::get( handle );
			StructPlayer *pFriend = *pit;

			// 온라인인 친구는 직접 변경해주고, 나머지는 DB 명령어에서 친구 목록 일괄 변경
			if( pFriend )
			{
				// 내가 친구로 등록했으니까 그 사람이 온라인이면 그 사람의 FriendOf 리스트에 이름을 바꿔줘야 함
				std::vector< std::string >::iterator itFriendOf = std::find( pFriend->m_vFriendOf.begin(), pFriend->m_vFriendOf.end(), GetName() );

				if( itFriendOf != pFriend->m_vFriendOf.end() )
				{
					(*itFriendOf) = szName;
				}
			}
		}

		for( std::vector< std::string >::iterator it = m_vFriendOf.begin() ; it != m_vFriendOf.end() ; ++it )
		{
			AR_HANDLE handle = StructPlayer::FindPlayer( (*it).c_str() );
			StructPlayer::iterator pit = StructPlayer::get( handle );
			StructPlayer *pFriend = *pit;

			// 온라인인 친구는 직접 변경해주고, 나머지는 DB 명령어에서 친구 목록 일괄 변경
			if( pFriend )
			{
				// 나를 친구로 등록했으니까 그 사람이 온라인이면 그 사람의 Friend 리스트에 이름을 바꿔줘야 함
				std::vector< std::string >::iterator itFriend = std::find( pFriend->m_vFriend.begin(), pFriend->m_vFriend.end(), GetName() );

				if( itFriend != pFriend->m_vFriend.end() )
				{
					// 자신을 상대방의 친구 목록에서 삭제해야 하는 경우에는 삭제, 아니면 새로운 이름으로 갱신
					if( bRemoveFromFriendAndDenial )
						pFriend->m_vFriend.erase( itFriend );
					else
						(*itFriend) = szName;

					// s_hsPlayerList에는 아직 변경되지 않았는데 변경된 이름을 포함한 친구 목록을 날리므로
					// 이름을 변경하고 있는 친구에 대해서는 무조건적으로 오프라인으로 나타남
					SendFriendsList( pFriend );
				}
			}
		}
	}

	{
		THREAD_SYNCHRONIZE( s_csPlayerName );

		std::string * pName = NULL;
		if( s_hsPlayerName.lookup( GetPlayerUID(), pName ) )
		{
			(*pName) = szName;
		}
	}

	{
		THREAD_SYNCHRONIZE( g_PlayerListlock );

		int code_page = ENV().GetInt( "CodePage", CP_ACP );
		char szUpperName[UPPERCASE_NAME_BUFFER_SIZE];
		XStringUtil::ToUpperThroughWChar( szUpperName, _countof( szUpperName ), GetName(), strlen( GetName() ), code_page );
		s_hsPlayerList.erase( szUpperName );

		SetName( szName );

		XStringUtil::ToUpperThroughWChar( szUpperName, _countof( szUpperName ), GetName(), strlen( GetName() ), code_page );
		s_hsPlayerList.add( szUpperName, GetHandle() );
	}

	// 나를 친구로 등록한 유저들에게 온라인으로 상태 갱신
	SendStatusMessageToFriendOfPlayer( this, true );
}

void StructPlayer::SetName( const char *szName )
{
	s_strcpy( m_szName, _countof( m_szName ), szName );
}

void StructPlayer::SetCreatureSpeed( unsigned char speed )
{
	if( HasRidingState() || IsRiding() )
	{
		m_Attribute.fRidingSpeed = speed;
	}
	else
	{
		m_Attribute.fMoveSpeed = speed;
	}
}

bool StructPlayer::IsHunter() const
{
	int nJobID = GetJobId();

	if( !nJobID )
	{
		switch( m_nRace )
		{
		case JobInfo::GAIA : nJobID = JobInfo::GAIA_BASIC_JOB; break;
		case JobInfo::DEVA : nJobID = JobInfo::DEVA_BASIC_JOB; break;
		case JobInfo::ASURA : nJobID = JobInfo::ASURA_BASIC_JOB; break;
		default: break;
		}
	}

	const JobInfo *pJobInfo = GameContent::GetJobInfo( nJobID );
	return pJobInfo ? pJobInfo->job_class == JobInfo::HUNTER : false;
}

bool StructPlayer::IsFighter() const
{
	int nJobID = GetJobId();

	if( !nJobID )
	{
		switch( m_nRace )
		{
		case JobInfo::GAIA : nJobID = JobInfo::GAIA_BASIC_JOB; break;
		case JobInfo::DEVA : nJobID = JobInfo::DEVA_BASIC_JOB; break;
		case JobInfo::ASURA : nJobID = JobInfo::ASURA_BASIC_JOB; break;
		default: break;
		}
	}

	const JobInfo *pJobInfo = GameContent::GetJobInfo( nJobID );
	return pJobInfo ? pJobInfo->job_class == JobInfo::FIGHTER : false;
}

bool StructPlayer::IsMagician() const
{
	int nJobID = GetJobId();

	if( !nJobID )
	{
		switch( m_nRace )
		{
		case JobInfo::GAIA : nJobID = JobInfo::GAIA_BASIC_JOB; break;
		case JobInfo::DEVA : nJobID = JobInfo::DEVA_BASIC_JOB; break;
		case JobInfo::ASURA : nJobID = JobInfo::ASURA_BASIC_JOB; break;
		default: break;
		}
	}

	const JobInfo *pJobInfo = GameContent::GetJobInfo( nJobID );
	return pJobInfo ? pJobInfo->job_class == JobInfo::MAGICIAN : false;
}

bool StructPlayer::IsSummoner() const
{
	int nJobID = GetJobId();

	if( !nJobID )
	{
		switch( m_nRace )
		{
		case JobInfo::GAIA : nJobID = JobInfo::GAIA_BASIC_JOB; break;
		case JobInfo::DEVA : nJobID = JobInfo::DEVA_BASIC_JOB; break;
		case JobInfo::ASURA : nJobID = JobInfo::ASURA_BASIC_JOB; break;
		default: break;
		}
	}

	const JobInfo *pJobInfo = GameContent::GetJobInfo( nJobID );
	return pJobInfo ? pJobInfo->job_class == JobInfo::SUMMONER : false;
}

AR_HANDLE StructPlayer::FindPlayer( const char *szName )
{
	THREAD_SYNCRONIZE( g_PlayerListlock );

	AR_HANDLE handle = 0;

	char szUpperName[UPPERCASE_NAME_BUFFER_SIZE];
	XStringUtil::ToUpperThroughWChar( szUpperName, _countof( szUpperName ), szName, strlen( szName ), ENV().GetInt( "CodePage", CP_ACP ) );
	s_hsPlayerList.lookup( szUpperName , handle );

	return handle;
}

void StructPlayer::EraseFromPlayerList( const char *szName )
{
	THREAD_SYNCRONIZE( g_PlayerListlock );

	char szUpperName[UPPERCASE_NAME_BUFFER_SIZE];
	XStringUtil::ToUpperThroughWChar( szUpperName, _countof( szUpperName ), szName, strlen( szName ), ENV().GetInt( "CodePage", CP_ACP ) );

	s_hsPlayerList.erase( szUpperName );
}

void StructPlayer::UnRegisterAccount( const char *szName )
{
	THREAD_SYNCRONIZE( g_PlayerListlock );
	struct IStreamSocketConnection * pConn;
	if( s_hsAccountList.lookup( szName, pConn) )
	{
		s_hsAccountList.erase( szName );	
	}
}

bool StructPlayer::RegisterAccount( struct IStreamSocketConnection * pConn, const char *szName )
{
	THREAD_SYNCRONIZE( g_PlayerListlock );

	if( !pConn->IsConnected() ) return false;

	struct IStreamSocketConnection *pTmpConn = NULL;

	s_hsAccountList.lookup( szName , pTmpConn );

	if( pTmpConn ) return false;

	s_hsAccountList.add( szName, pConn );

	return true;
}

IStreamSocketConnection * StructPlayer::GetConnectionByAccount( const char * szAccount )
{
	THREAD_SYNCRONIZE( g_PlayerListlock );

	IStreamSocketConnection * pConn = NULL;

	s_hsAccountList.lookup( szAccount , pConn );

	return pConn;
}

const bool StructPlayer::IsRegisteredAccount( const char * szAccount )
{
	THREAD_SYNCRONIZE( g_PlayerListlock );

	IStreamSocketConnection * pConn = NULL;

	return s_hsAccountList.lookup( szAccount, pConn );
}

const bool StructPlayer::SetRegisteredConnectionByAccount( const char *szAccount, struct IStreamSocketConnection * pConn )
{
	THREAD_SYNCRONIZE( g_PlayerListlock );

	IStreamSocketConnection * pPrevConn = NULL;

	if( !s_hsAccountList.lookup( szAccount, pPrevConn ) )
		return false;

	return s_hsAccountList.modify( szAccount, pConn );
}

void StructPlayer::Login()
{
	assert( !m_bIsLogin );

	if( m_bIsLogin ) return;

	m_nLastUpdateTime = m_nLoginTime = GetArTime();
	m_nLastContinuousPlayTimeProcTime = m_nLastUpdateTime;
	m_nLastTimeBasedEventTimeScript = m_nLastTimeBasedEventTimeDB = m_nLastPlayTimeUpdateTime = m_nLastUpdateTime;
	m_bIsLogin = true;
	m_bHasLogined = true;

	{
		THREAD_SYNCRONIZE( g_PlayerListlock );

		g_vPlayerList.push_back( this );
		m_nPlayerListIndex = static_cast< int >( g_vPlayerList.size() - 1 );

		char szUpperName[UPPERCASE_NAME_BUFFER_SIZE];
		XStringUtil::ToUpperThroughWChar( szUpperName, _countof( szUpperName ), GetName(), strlen( GetName() ), ENV().GetInt( "CodePage", CP_ACP ) );
		s_hsPlayerList.add( szUpperName, GetHandle() );
	}

	// 퀘스트 처리
	{
		ARCADIA_LOCK( ArcadiaServer::Instance().LockObjectWithVisibleRange( this ) );

		std::vector< StructQuest* >::iterator it;
		std::vector< StructQuest* > vQuestList;
		m_QuestManager.GetRelatedQuest( vQuestList );
		for( it = vQuestList.begin(); it != vQuestList.end(); ++ it )
		{
			updateQuestStatus( *it );
		}
	}

	// party matching
	m_party_matching = NULL;
	m_party_matching_leader_name.clear();
}

bool StructPlayer::CanLogoutNow()
{
	// 로그아웃 대기 시간이 0으로 세팅되어 있다면 언제나 로그아웃 가능
	if( !GameRule::nLogoutTimer )
		return true;

	bool bRet = false;

	if ( IsPKOn() )
		bRet = false;
	else if ( IsInTown() )
		bRet = true;
	else if ( IsBloodyCharacter() || IsDemoniacCharacter() )
		bRet = false;
	else
		bRet = !GameRule::bLimitFieldLogout;

	// 즉시 로그아웃 불가능 상황이지만 대기 시간만큼 기다렸다면 가능
	// 로그아웃 가능 시간: 로그아웃 타이머 요청 시점 + GameRule::nLogoutTimer 부터 GameRule::LOGOUT_AVAILABLE_DURATION / 100 초 동안
	if( !bRet && GetLastLogoutRequestedTime() )
	{
		AR_TIME t = GetArTime();
		if( t >= GetLastLogoutRequestedTime() + GameRule::nLogoutTimer && t <= GetLastLogoutRequestedTime() + GameRule::nLogoutTimer + GameRule::LOGOUT_AVAILABLE_DURATION )
			bRet = true;
	}

	return bRet;
}

void StructPlayer::LogoutNowWithAccount( const int nCallerIdx )
{
	// 락이 하나라도 걸린 채로 호출하면 바로 데드락 ㄳ

	assert( IsInWorld() );

	StructPlayer::UnRegisterAccount( GetAccountName() );
	SendLogoutToAuth( GetAccountName(), GetContinuousPlayTime(), 2 );
	{
		// StructPlayer::pConnection으로 pTag를 찾으려면 아래와 같은 락 삽질을 동반한다... -_ -;

		// pConnection의 pTag를 얻기 위한 락을 먼저 걸기(g_ConnectionTagLock -> 지역 락 순서여야 함)
		THREAD_SYNCRONIZE( g_ConnectionTagLock );

		_CONNECTION_TAG * pTag = NULL;

		{
			// LogoutNow에서 pConnection 을 NULL로 세팅하는 것을 방지하기 위해 지역 락 걸기
			ARCADIA_LOCK( ArcadiaServer::Instance().LockObjectWithVisibleRange( this ) );

			// 접속이 비정상 종료되어 ConnectionCloser::onProcess에서 StructPlayer::OnConnectionClosed를
			// 호출했었을 경우 pConnection이 NULL이 되어 있음(이 경우는 pTag관련 동작이 불필요)
			// 이미 로그아웃 처리된 경우도 마찬가지이지만 로그아웃 된 유저가 애초에 여기로 들어오면
			// 그 메커니즘 자체가 잘못 동작하고 있음을 의미함
			if( pConnection )
			{
				pTag = static_cast< _CONNECTION_TAG * >( pConnection->GetTag() );
			}
		}

		if( pTag && pTag->szAccountName[0] && pTag->bAuthByAuthServer )
		{
			pTag->bAuthByAuthServer = false;
		}
	}

	LogoutNow( nCallerIdx );
}

void StructPlayer::OnConnectionClosed( struct _CONNECTION_TAG * pTag )
{
	if( GameRule::bUseLoginLogoutDebug )
	{
		_ctprint( "OnConnectionClosed (%s:%s)\n", GetAccountName(), GetName() );
		FileLogHandler::GetFileLogHandler()->LogStringEx( NULL, "LoginLogout_Debug", "OnConnectionClosed (%s:%s)", GetAccountName(), GetName() );
	}

	if( !IsLogin() )
	{
		assert( 0 );
		_cprint( "OnConnectionClosed is called with a player not logged in.[%s:%s]\n", GetAccountName(), GetName() );
		FILELOG( "OnConnectionClosed is called with a player not logged in.[%s:%s]", GetAccountName(), GetName() );

		// 로그인 실패한 채로 접속 끊기면 이리 옴. 인증 서버에게 로그아웃 시켜야 한다고 알려줘야 함.
		if( pTag && pTag->szAccountName[0] && pTag->bAuthByAuthServer )
		{
			StructPlayer::UnRegisterAccount( pTag->szAccountName );
			SendLogoutToAuth( pTag->szAccountName, GetContinuousPlayTime(), 3 );
			pTag->bAuthByAuthServer = false;
		}

		return;
	}

	if( !IsInWorld() )
	{
		_cprint( "OnConnectionClosed is called with a player not in world.[%s:%s]\n", GetAccountName(), GetName() );
		FILELOG( "OnConnectionClosed is called with a player not in world.[%s:%s]", GetAccountName(), GetName() );
		return;
	}

	// 로그아웃 타이머가 세팅되어 있지 않다면 onProcess에서 로그아웃 되도록 타이머 세팅
	if( !GetLogoutTimer() )
	{
		// 즉시 로그아웃 가능할 경우 onProcess에서 바로 로그아웃 되도록 타이머 세팅
		// 즉시 로그아웃 불가능하면 로그아웃 타이머 시간 이후에 로그아웃 되도록 세팅
		SetLogoutTimer( GetArTime() + ( CanLogoutNow() ? 0 : GameRule::nLogoutTimer ) );
	}
}

void StructPlayer::LogoutNow( const int nCallerIdx )
{
	if( GameRule::bUseLoginLogoutDebug )
	{
		_ctprint( "LogoutNow (%s, %s, %d)\n", GetAccountName(), GetName(), nCallerIdx );
		FileLogHandler::GetFileLogHandler()->LogStringEx( NULL, "LoginLogout_Debug", "LogoutNow (%s, %s, %d)", GetAccountName(), GetName(), nCallerIdx );
	}

	{
		for( std::set< int >::const_iterator it = m_stEventAreaID.begin(); it != m_stEventAreaID.end(); ++it )
		{
			LUA()->RunString( GameContent::GetEventAreaLeaveHandler( (*it) ) );
		}

		m_stEventAreaID.clear();
	}

	// 헌터홀릭 던전에서 나가는 건 소환수를 월드에서 제거하기 전에 처리되어야 함(경험치 보상되는 경우 배분 처리)
	int nHuntaholicID = HuntaholicManager::Instance().GetHuntaholicID( GetPos() );
	if( nHuntaholicID && IsInParty() )
	{
		if( HuntaholicManager::Instance().IsHuntaholicDungeon( GetPos() ) )
		{
			HuntaholicManager::Instance().QuitHunting( nHuntaholicID, this, true, ( IsDead() ) ? HuntaholicManager::HUNTING_RESULT_FAILED_BY_DEATH : HuntaholicManager::HUNTING_RESULT_RETIRED );
		}
		else
		{
			HuntaholicManager::Instance().LeaveInstanceDungeon( nHuntaholicID, this );
		}
	}

	int nInstanceDungeonID = InstanceDungeonManager::Instance().GetInstanceDungeonID( GetPos() );
	if( nInstanceDungeonID )
	{
		InstanceDungeonManager::Instance().LeaveInstanceDungeon( nInstanceDungeonID, this );
	}

	// LogoutNow 관련 함수 호출 전에 배틀 아레나 관련 처리가 되지 않은 경우 처리
	if( GetBattleArenaID() )
	{
		ARCADIA_LOCK( ArcadiaServer::Instance().LockObjectWithVisibleRange( this ) );

		// 캐릭터 선택화면으로 돌아가는 거나 게임 종료 버튼을 사용하는 경우에는 관련 패킷을 받은 시점(onReturnLobby, onLogout)에 처리를 먼저 하기 때문에
		// 여기 오는 건 대개 예상 못 했던 문제나 오류로 인해 접속이 끊기는 경우로 간주하고, 접속 종료에 의한 이탈로 처리함.
		// * 이런 식이 아니면 유저를 로그아웃시키는 모든 케이스마다 로그아웃 유형을 분류하고 처리해야 하는데
		//   예를 들면 중복 접속의 경우 이전 접속이 유효한 접속이었는지, 아니면 접속이 끊겼는데 서버에서 감지를 못한 건지
		//   확인할 방법이 없음. 이런 모호한 경우들에 대해 일일히 룰을 세우는 것 보다는 접속 종료 처리가 아니라
		//   페널티를 주고 바로 이탈시킬 부분만 따로 처리해주고, 그 외의 모든 경우를 접속 종료 처리되도록 하는 것이 차라리 안전하다고 생각됨.
		BattleArenaManager::Instance().OnDisconnect( this );

		// 이탈 처리가 완료되어 있어야 함
		assert( !GetBattleArenaID() );
	}

	if( IsInWorld() )
	{
		RemoveAllSummonFromWorld();
		RemoveAllPetFromWorld();
	}

	if( GetCompeteID() )
	{
		CompeteManager::Instance().RetireCompeteWithPlayer( this, COMPETE_END_BY_LOGOUT );
	}

	if( pConnection )
	{
		THREAD_SYNCRONIZE( g_ConnectionTagLock );

		// 락 걸린 채로 pConnection NULL 체크 한 번 더
		if( pConnection )
		{
			_CONNECTION_TAG * pTag = static_cast< _CONNECTION_TAG * >( pConnection->GetTag() );

			if( pConnection->GetTag() )
			{
				pTag->nContinuousPlayTime = GetContinuousPlayTime();
				pTag->nContinuousLogoutTime = GetContinuousLogoutTime();

				pTag->pPlayer = NULL;
			}
		}
	}

	{
		ARCADIA_LOCK( ArcadiaServer::Instance().LockObjectWithVisibleRange( this ) );

		if( IsInWorld() )
		{
			RemovePlayerFromWorld( this );

			quadTreeItem.RemoveMe();
		}

		if( IsUsingSkill() )
		{
			CancelSkill();
		}

		// 테이밍 취소
		if( GetTamingTarget() )
		{
			StructMonster::iterator it = StructMonster::get( GetTamingTarget() );

			if( (*it) )
			{
				ClearTamer( static_cast< StructMonster * >(*it) );
			}
		}

		// 이동처리
		if ( IsMoving() )
			Step( GetArTime() );

		// DB에 기록
		// Save 전에 해야 스킬 쿨타임 0짜리도 DB에 저장 함
		m_bIsLogin = false;
		Save();

		ChannelManager::LeavePlayerFromLayer( GetLayer() );

		DBQuery( new DB_Logout( this ) );

		if( GetLocationId() )
		{
			WorldLocationManager::Instance().RemoveFromLocation( this );
			m_pWorldLocation = NULL;
		}

		int nPartyID = GetPartyID();
		if( nPartyID )
		{	
			PartyManager::GetInstance().onLogout( nPartyID, this );
			PrintfLinkedPartyChatMessage( false, CHAT_PARTY_SYSTEM, "@PARTY", nPartyID, "LOGOUT|%s|", GetAlias() );
		}

		if( GetGuildID() != 0 )
		{	
			GuildManager::GetInstance().onLogout( GetGuildID(), this );
			PrintfGuildChatMessage( CHAT_GUILD_SYSTEM, GetGuildID(), "LOGOUT|%s|", GetName() );
		}

		SendStatusMessageToFriendOfPlayer( this, false );

		// { 로그아웃
		if( m_hTarget )
		{
			StructCreature::iterator it = StructCreature::get( m_hTarget );
			StructCreature *pObj = *it;

			if( pObj )
			{
				pObj->RemoveAimer( GetHandle() );
			}

			m_hTarget = 0;
		}

		m_nLogoutTime = GetArTime();

		// 트레이드 취소
		if( IsTrading() )
		{
			StructPlayer::iterator itTradeTarget = StructPlayer::get( GetTradeTarget() );
			StructPlayer * pTradeTarget = (*itTradeTarget);

			if( pTradeTarget )
				pTradeTarget->CancelTrade( true );

			CancelTrade();		
		}

		RemoveAllHate();

		LOG::Log11N4S( LM_CHARACTER_LEAVE, GetAccountID(), GetSID(), GetLevel(), GetJobId(), GetPCBangMode(), GetStamina(), GetGold().GetRawData(), GetLayer(), GetX(), GetY(), ( m_nLogoutTime - m_nLoginTime ) / 100, GetAccountName(), LOG::STR_NTS, GetName(), LOG::STR_NTS, "", 0, "", 0 );
		LOG::Log11N4S( LM_CHARACTER_INFO, GetAccountID(), GetSID(), GetLevel(), GetJobLevel(), GetJobPoint(), GetJobId(), GetGold().GetRawData(), GetStorageGold().GetRawData(), GetChaos(), GetImmoralPoint(), GetEXP(), GetAccountName(), LOG::STR_NTS, GetName(), LOG::STR_NTS, "", 0, "", 0 );

		pConnection = NULL;	// 이후에 pConnection으로 쏘는 건; 뭐 알아서 잘 되겠지. NULL체크 할테니.-ㅅ-;;;

		ArcadiaServer::Instance().DeleteObject( this );
	}

	// party matching
	if(m_party_matching)
	{
		CPartyMatchingManager::GetInstance()->Remove(this);
	}
}


void StructPlayer::RemoveAllSummonFromWorld()
{
	// 소환수 월드에서 제거
	std::vector< StructSummon* >::iterator sit;
	for( sit = m_vSummonList.begin(); sit != m_vSummonList.end(); ++sit )
	{	
		if( !(*sit)->IsInWorld() ) continue;

		ARCADIA_LOCK( ArcadiaServer::Instance().LockObjectWithVisibleRange( *sit ) );

		if( !(*sit)->IsInWorld() ) continue;

		RemoveSummonFromWorld( *sit );
	}

	// 역소환 대기 중이던 소환수 월드에서 제거
	procPendingUnSummon();
}

void StructPlayer::RemoveAllPetFromWorld()
{
	for( std::vector< StructPet * >::iterator it = m_vPetList.begin() ; it != m_vPetList.end() ; ++it )
	{
		if( !(*it)->IsInWorld() ) continue;

		ARCADIA_LOCK( ArcadiaServer::Instance().LockObjectWithVisibleRange( (*it) ) );

		if( !(*it)->IsInWorld() ) continue;

		RemovePetFromWorld( *it );
	}

	// 역소환 대기 중이던 펫 월드에서 제거
	procPendingUnSummonPet();
}

void StructPlayer::DoEachPlayer( ArObjectFunctor & _fo )
{
	THREAD_SYNCRONIZE( g_PlayerListlock );

	std::vector< StructPlayer* >::iterator it;
	for( it = g_vPlayerList.begin(); it != g_vPlayerList.end(); ++it )
	{
		// 1. IsLogin 검사: LogoutNow 호출 이후 ~StructPlayer 호출 이전 사이의 플레이어는 대상에서 제외
		// 2. s_hsAccountList 검사: 접속 종료가 로그아웃보다 먼저 일어나는 경우(SetLogoutTimer만 하는 경우 등)
		//                          pConnection != NULL 이지만 delete 되어 있을 수 있는 경우 제외
		//                          pConnection != NULL 이면서 delete 되었다 하더라도 s_hsAccountList의 connection은 NULL 또는 없음을 보장 함(g_PlayerListlock 덕분에)
		IStreamSocketConnection * pConn = NULL;
		if( (*it)->IsLogin() && s_hsAccountList.lookup( (*it)->GetAccountName(), pConn ) && pConn )
		{
			_fo( *it );
		}
	}
}

void StructPlayer::DoEachPartyPlayer( ArObjectFunctor & _fo )
{
	THREAD_SYNCRONIZE( g_PlayerListlock );

	{
		THREAD_SYNCRONIZE(PartyManager::GetInstance().GetLock());

		const PartyManager::PartyInfo* pPartyInfo = PartyManager::GetInstance().getPartyInfo(GetPartyID());
		if(NULL == pPartyInfo)
		{
			return;
		}

		std::vector<PartyManager::PartyMemberTag>::const_iterator iter;
		for(iter = pPartyInfo->vMemberNameList.begin(); iter != pPartyInfo->vMemberNameList.end(); ++iter)
		{
			// 1. IsLogin 검사: LogoutNow 호출 이후 ~StructPlayer 호출 이전 사이의 플레이어는 대상에서 제외
			// 2. s_hsAccountList 검사: 접속 종료가 로그아웃보다 먼저 일어나는 경우(SetLogoutTimer만 하는 경우 등)
			//                          pConnection != NULL 이지만 delete 되어 있을 수 있는 경우 제외
			//                          pConnection != NULL 이면서 delete 되었다 하더라도 s_hsAccountList의 connection은 NULL 또는 없음을 보장 함(g_PlayerListlock 덕분에)
			IStreamSocketConnection * pConn = NULL;
			StructPlayer* pPlayer = *StructPlayer::get(StructPlayer::FindPlayer(iter->strName.c_str()));
			if(NULL == pPlayer)
			{
				continue;
			}
			if(pPlayer->IsLogin() && s_hsAccountList.lookup(pPlayer->GetAccountName(), pConn ) && pConn)
			{
				_fo(pPlayer);
			}
		}
	}
}

void StructPlayer::DoEachGuildPlayer( ArObjectFunctor & _fo )
{
	THREAD_SYNCRONIZE( g_PlayerListlock );

	{
		THREAD_SYNCRONIZE(GuildManager::GetInstance().GetLock());

		const GuildManager::GuildInfo* pGuildInfo = GuildManager::GetInstance().getGuildInfo(GetGuildID());
		if(NULL == pGuildInfo)
		{
			return;
		}

		std::vector<AR_HANDLE>::const_iterator iter;
		for(iter = pGuildInfo->vOnlineList.begin(); iter != pGuildInfo->vOnlineList.end(); ++iter)
		{
			// 1. IsLogin 검사: LogoutNow 호출 이후 ~StructPlayer 호출 이전 사이의 플레이어는 대상에서 제외
			// 2. s_hsAccountList 검사: 접속 종료가 로그아웃보다 먼저 일어나는 경우(SetLogoutTimer만 하는 경우 등)
			//                          pConnection != NULL 이지만 delete 되어 있을 수 있는 경우 제외
			//                          pConnection != NULL 이면서 delete 되었다 하더라도 s_hsAccountList의 connection은 NULL 또는 없음을 보장 함(g_PlayerListlock 덕분에)
			IStreamSocketConnection * pConn = NULL;
			StructPlayer* pPlayer = *StructPlayer::get(*iter);
			if(NULL == pPlayer)
			{
				continue;
			}
			if(pPlayer->IsLogin() && s_hsAccountList.lookup(pPlayer->GetAccountName(), pConn ) && pConn)
			{
				_fo(pPlayer);
			}
		}
	}
}

void StructPlayer::DoPlayer( AR_HANDLE handle, ArObjectFunctor & _fo )
{
	THREAD_SYNCRONIZE( g_PlayerListlock );

	// 함수 호출 측에서 유효한 handle 값을 넘겼어도 DoPlayer 함수 안에서 GameObject::raw_get로 GameObject *을 얻었을 때 NULL이 나올 수 있음.(지역락이 걸리지 않은 경우에만)
	// handle의 유효성을 체크한 시점과 아래에서 GameOBject::raw_get 함수가 호출되는 시점 사이에 DeleteObject가 호출되어서
	// 메모리상에서는 삭제되지 않았더라도(iterator의 ref_count 등에 의해서) raw_get에서 IsDeleteRequested() == true 면 포인터를 반환하지 않음
	// 특히 여기는 StructPlayer에 대해서만 사용되므로 예를 들어보자면, 특정 유저가 대상 주변에 지역락을 걸고 호출하는 거라면 상관없지만
	// (지역락이 걸려 있으므로 대상에 대해 LogoutNow -> DeleteObject가 호출될 수 없음) 특정 유저가 자신 주변에만 지역락을 걸고 대상의 위치와
	// 상관없이 실행되는 함수를 처리하는 거라면(귓말, 각종 초대 등등) ArObjectFuctor가 NULL 포인터를 받을 수도 있음.
	// 따라서 raw_get의 결과가 NULL 포인터가 넘어올 수 있다는 걸 가정하고 NULL 체크를 해야 함.

	// 거기에 추가로 로그인 상태인지 체크까지 해주면 더 안전할듯...
	// * DoEachPlayer에서 하듯이 s_hsAccountList 체크로 StructPlayer::pConnection의 NULL 체크까지 해주면 좋겠지만 성능에 영향을 다소 끼치게될 것 같아서 패스.
	//   (이 함수 자체가 호출되는 경로는 현재는 SendChatMessage 밖에 없지만, SendChatMessage가 여기저기서 많이 쓰임)

	GameObject * pTarget = GameObject::raw_get( handle );
	if( !pTarget || !pTarget->IsPlayer() || !static_cast< StructPlayer *>( pTarget )->IsLogin() )
		return;

	_fo( pTarget );
}

bool StructPlayer::IsDeleteable()
{
	THREAD_SYNCRONIZE( m_bQueryLock );

	//if( static_cast< XIOCPConnection* >( pConnection )->GetVar() ) return false;

	// 스케쥴러에 등록되어 있으면 삭제 불가 && 다른 쓰레드에서 참조중이면 삭제 불가
	if( !GameObject::IsDeleteable() ) return false;

	// 로그인한적이 있는데 로그아웃을 안했다면 삭제 불가
	//if( m_bIsLogin && !m_nLogoutTime ) return false;

	// DB 쿼리 미결된게 있다면 삭제 불가
	if( !m_lQueryList.empty() ) return false;

	// 로그아웃한지 5초이상 지났을경우에만 삭제 가능하도록 하자.
	//if( m_nLogoutTime + 500 > GetArTime() ) return false;

	return true;
}

bool StructPlayer::TranslateWearPosition( ItemBase::ItemWearType & pos, struct StructItem *pItem, std::vector< int > * vpOverlappItemList )
{
	// 스왑용 아이템은 성능이 적용되지 않을 뿐더러 스왑하는 시점에 다시 한 번 검사하기 때문에 별도로 검사하지 않는다.
	if( pos >= ItemBase::MAX_ITEM_WEAR && pos < ItemBase::MAX_SPARE_ITEM_WEAR ) return true;

	if( !StructCreature::TranslateWearPosition( pos, pItem, vpOverlappItemList ) ) return false;

	if( pItem->GetInstanceFlag().IsOn( ItemInstance::ITEM_FLAG_CARD ) ) return false;

	bool bWearable = false;

	if( pItem->GetItemBase().nLimit & ItemBase::LIMIT_HUNTER && IsHunter() ) bWearable = true;
	if( pItem->GetItemBase().nLimit & ItemBase::LIMIT_FIGHTER && IsFighter() ) bWearable = true;
	if( pItem->GetItemBase().nLimit & ItemBase::LIMIT_MAGICIAN && IsMagician() ) bWearable = true;
	if( pItem->GetItemBase().nLimit & ItemBase::LIMIT_SUMMONER && IsSummoner() ) bWearable = true;

	if( !bWearable ) 
		return false;

	if( GetRace() == JobInfo::GAIA )	if( !( pItem->GetItemBase().nLimit & ItemBase::LIMIT_GAIA ) ) return false;
	if( GetRace() == JobInfo::DEVA )	if( !( pItem->GetItemBase().nLimit & ItemBase::LIMIT_DEVA ) ) return false;
	if( GetRace() == JobInfo::ASURA )	if( !( pItem->GetItemBase().nLimit & ItemBase::LIMIT_ASURA ) ) return false;

	// 직업 차수 제한
	if( ( 1 << GetJobDepth() & pItem->GetItemBase().nJobDepth ) == 0 )
		return false;

	if( pos == ItemBase::WEAR_LEFTHAND && pItem->IsBullet() )
	{
		if( !m_anWear[ ItemBase::WEAR_RIGHTHAND ] )
		{
			return false;
		}

		if( m_anWear[ ItemBase::WEAR_RIGHTHAND ]->GetItemClass() != ItemBase::CLASS_CROSSBOW &&
			m_anWear[ ItemBase::WEAR_RIGHTHAND ]->GetItemClass() != ItemBase::CLASS_LIGHT_BOW &&
			m_anWear[ ItemBase::WEAR_RIGHTHAND ]->GetItemClass() != ItemBase::CLASS_HEAVY_BOW )
		{
			return false;
		}
	}

	if( pos == ItemBase::WEAR_LEFTHAND && pItem->IsWeapon() )
	{
		if( !m_anWear[ ItemBase::WEAR_RIGHTHAND ] )
		{
			return false;
		}

		if( m_anWear[ ItemBase::WEAR_RIGHTHAND ]->GetItemClass() == ItemBase::CLASS_ONEHAND_SWORD )
		{
			// 양손칼 장착은 듀얼 소드 익스퍼트가 있어야만 가능
			if( pItem->GetItemClass() == ItemBase::CLASS_ONEHAND_SWORD )
			{
				int expert_level = GetCurrentPassiveSkillLevel( StructSkill::SKILL_DUAL_SWORD_EXPERT );

				if( expert_level < 1 )
				{
					expert_level = GetCurrentPassiveSkillLevel( StructSkill::SKILL_TWIN_BLADE_EXPERT );
				}

				if( expert_level < 1 )
				{
					pos = ItemBase::WEAR_WEAPON;
				}
			}
			else
			{
				pos = ItemBase::WEAR_WEAPON;
			}
		}
		else if( m_anWear[ ItemBase::WEAR_RIGHTHAND ]->GetItemClass() == ItemBase::CLASS_DAGGER )
		{
			// 양손 대거 장착은 더블 대거 익스퍼트가 있어야만 가능
			if( pItem->GetItemClass() == ItemBase::CLASS_DAGGER )
			{
				int expert_level = GetCurrentPassiveSkillLevel( StructSkill::SKILL_TWIN_BLADE_EXPERT );

				if( expert_level < 1 )
				{
					pos = ItemBase::WEAR_WEAPON;
				}
			}
			else
			{
				pos = ItemBase::WEAR_WEAPON;
			}
		}
		else if( m_anWear[ ItemBase::WEAR_RIGHTHAND ]->GetItemClass() == ItemBase::CLASS_ONEHAND_AXE )
		{
			// 양손 도끼 장착은 쌍도끼 연마가 있어야만 가능
			if( pItem->GetItemClass() == ItemBase::CLASS_ONEHAND_AXE )
			{
				int expert_level = GetCurrentPassiveSkillLevel( StructSkill::SKILL_TWIN_AXE_EXPERT );

				if( expert_level < 1 )
				{
					pos = ItemBase::WEAR_WEAPON;
				}
			}
			else
			{
				pos = ItemBase::WEAR_WEAPON;
			}
		}
		else if (m_anWear[ItemBase::WEAR_RIGHTHAND]->GetItemClass() == ItemBase::CLASS_CROSSBOW)
		{
			// 양손 도끼 장착은 쌍도끼 연마가 있어야만 가능
			if (pItem->GetItemClass() == ItemBase::CLASS_CROSSBOW)
			{
				int expert_level = GetCurrentPassiveSkillLevel(StructSkill::SKILL_TWIN_CROSSBOW_EXPERT);

				if (expert_level < 1)
				{
					pos = ItemBase::WEAR_WEAPON;
				}
			}
			else
			{
				pos = ItemBase::WEAR_WEAPON;
			}
		}
		else
		{
			pos = ItemBase::WEAR_WEAPON;
		}
	}

	if( pos == ItemBase::WEAR_DECO_SHIELD )
	{
		// -_ -;;; 꾸미기 무기인 걸 알 수 있게 ITEM_GROUP 리뉴얼이 필요함. 일반 장비가 무기/의상/방패 등등으로 나뉘듯이... IsWeapon처럼 IsDecoWeapon을 추가할 수 있도록...
		if( pItem->GetItemClass() == ItemBase::CLASS_DECO_ONEHAND_SWORD	||
			pItem->GetItemClass() == ItemBase::CLASS_DECO_TWOHAND_SWORD	||
			pItem->GetItemClass() == ItemBase::CLASS_DECO_DAGGER		||
			pItem->GetItemClass() == ItemBase::CLASS_DECO_TWOHAND_SPEAR	||
			pItem->GetItemClass() == ItemBase::CLASS_DECO_TWOHAND_AXE	||
			pItem->GetItemClass() == ItemBase::CLASS_DECO_ONEHAND_MACE	||
			pItem->GetItemClass() == ItemBase::CLASS_DECO_TWOHAND_MACE	||
			pItem->GetItemClass() == ItemBase::CLASS_DECO_HEAVY_BOW		||
			pItem->GetItemClass() == ItemBase::CLASS_DECO_LIGHT_BOW		||
			pItem->GetItemClass() == ItemBase::CLASS_DECO_CROSSBOW		||
			pItem->GetItemClass() == ItemBase::CLASS_DECO_ONEHAND_STAFF	||
			pItem->GetItemClass() == ItemBase::CLASS_DECO_TWOHAND_STAFF	||
			pItem->GetItemClass() == ItemBase::CLASS_DECO_ONEHAND_AXE	)
		{
			if( m_anWear[ ItemBase::WEAR_LEFTHAND ] &&
				m_anWear[ ItemBase::WEAR_LEFTHAND ]->GetItemClass() != ItemBase::CLASS_ONEHAND_SWORD &&
				m_anWear[ ItemBase::WEAR_LEFTHAND ]->GetItemClass() != ItemBase::CLASS_DAGGER && 
				m_anWear[ ItemBase::WEAR_LEFTHAND ]->GetItemClass() != ItemBase::CLASS_ONEHAND_AXE &&
				m_anWear[ ItemBase::WEAR_LEFTHAND ]->GetItemClass() != ItemBase::CLASS_CROSSBOW )

			{
				pos = ItemBase::WEAR_DECO_WEAPON;
			}
			else if( !m_anWear[ ItemBase::WEAR_RIGHTHAND ] ||
				( m_anWear[ ItemBase::WEAR_RIGHTHAND ]->GetItemClass() != ItemBase::CLASS_ONEHAND_SWORD &&
				m_anWear[ ItemBase::WEAR_RIGHTHAND ]->GetItemClass() != ItemBase::CLASS_DAGGER &&
				m_anWear[ ItemBase::WEAR_RIGHTHAND ]->GetItemClass() != ItemBase::CLASS_ONEHAND_AXE &&
				m_anWear[ ItemBase::WEAR_LEFTHAND  ]->GetItemClass() != ItemBase::CLASS_CROSSBOW) )
			{
				pos = ItemBase::WEAR_DECO_WEAPON;
			}
			else if( !m_anWear[ ItemBase::WEAR_DECO_WEAPON ] )
			{
				pos = ItemBase::WEAR_DECO_WEAPON;
			}
			else
			{
				int expert_level = GetCurrentPassiveSkillLevel( StructSkill::SKILL_DUAL_SWORD_EXPERT );

				if( expert_level < 1 )
				{
					expert_level = GetCurrentPassiveSkillLevel( StructSkill::SKILL_TWIN_BLADE_EXPERT );
				}
				if( expert_level < 1 )
				{
					expert_level = GetCurrentPassiveSkillLevel( StructSkill::SKILL_TWIN_AXE_EXPERT );
				}
				if (expert_level < 1)
				{
					expert_level = GetCurrentPassiveSkillLevel(StructSkill::SKILL_TWIN_CROSSBOW_EXPERT);
				}

				if( expert_level < 1 )
				{
					pos = ItemBase::WEAR_DECO_WEAPON;
				}
			}
		}
		else
		{
			if( ( m_anWear[ ItemBase::WEAR_LEFTHAND ] && m_anWear[ ItemBase::WEAR_LEFTHAND ]->GetItemClass() != ItemBase::CLASS_SHIELD ) ||
				( m_anWear[ ItemBase::WEAR_RIGHTHAND ] && m_anWear[ ItemBase::WEAR_RIGHTHAND ]->IsTwoHandItem() ) )
			{
				return false;
			}
		}
	}

	// 두손장비는 실제로 자료구조상으로는 오른손에 들도록 한다.
	if( pItem->GetWearType() == ItemBase::WEAR_TWOHAND && pItem->GetItemClass() != ItemBase::CLASS_CROSSBOW )
	{
		pos = ItemBase::WEAR_WEAPON;
	}

	// 악세사리 위치조절
	if( pos == ItemBase::WEAR_TWOFINGER_RING ) pos = ItemBase::WEAR_RING;
	if( m_anWear[ ItemBase::WEAR_RING ] && m_anWear[ ItemBase::WEAR_RING ]->GetWearType() != ItemBase::WEAR_TWOFINGER_RING && pItem->GetWearType() == ItemBase::WEAR_RING )
	{
		pos = ItemBase::WEAR_SECOND_RING;
	}
	if (m_anWear[ItemBase::WEAR_EAR] && pItem->GetWearType() == ItemBase::WEAR_EAR)
	{
		pos = ItemBase::WEAR_EAR2;
	}

	// 가방이라면 성능을 비교해서 무게가 100% 넘지 않도록 제한
	if( pos == ItemBase::WEAR_BAG_SLOT && m_anWear[ ItemBase::WEAR_BAG_SLOT ] )
	{
		// 동일한 종류의 가방이라면 언제나 가능(다를 때만 추가 체크)
		if( m_anWear[ ItemBase::WEAR_BAG_SLOT ]->GetItemCode() != pItem->GetItemCode() )
		{
			int nCurrentVarIdx = -1;
			int nNewVarIdx = -1;

			const ItemBaseServer & current_bag_base = m_anWear[ ItemBase::WEAR_BAG_SLOT ]->GetItemBase();
			c_fixed10 current_bag_capacity;
			const ItemBaseServer & new_bag_base = pItem->GetItemBase();
			c_fixed10 new_bag_capacity;

			for( int i = 0 ; i < ItemBase::MAX_OPTION_NUMBER ; ++i )
			{
				if( current_bag_base.nOptType[ i ] == ITEM_EFFECT_PASSIVE::CARRY_WEIGHT )
					current_bag_capacity += current_bag_base.fOptVar1[ i ];
				if( new_bag_base.nOptType[i] == ITEM_EFFECT_PASSIVE::CARRY_WEIGHT )
					new_bag_capacity += new_bag_base.fOptVar1[ i ];
			}

			if( current_bag_base.pvEffectList && !current_bag_base.pvEffectList->empty() )
			{
				for( std::vector< EffectInfo * >::const_iterator it = current_bag_base.pvEffectList->begin() ; it != current_bag_base.pvEffectList->end() ; ++it )
				{
					EffectInfo * pEffect = (*it);

					if( pEffect->eType != EffectInfo::EFFECT_TYPE_OPTIONAL )
						continue;

					if( pEffect->nMinLevel && GetLevel() < pEffect->nMinLevel ) continue;
					if( pEffect->nMaxLevel && GetLevel() > pEffect->nMaxLevel ) continue;

					if( pEffect->fValue[ 0 ] == ITEM_EFFECT_PASSIVE::CARRY_WEIGHT )		current_bag_capacity += pEffect->fValue[ 1 ];
					if( pEffect->fValue[ 3 ] == ITEM_EFFECT_PASSIVE::CARRY_WEIGHT )		current_bag_capacity += pEffect->fValue[ 4 ];
					if( pEffect->fValue[ 6 ] == ITEM_EFFECT_PASSIVE::CARRY_WEIGHT )		current_bag_capacity += pEffect->fValue[ 7 ];
					if( pEffect->fValue[ 9 ] == ITEM_EFFECT_PASSIVE::CARRY_WEIGHT )		current_bag_capacity += pEffect->fValue[ 10 ];
				}
			}

			if( new_bag_base.pvEffectList && !new_bag_base.pvEffectList->empty() )
			{
				for( std::vector< EffectInfo * >::const_iterator it = new_bag_base.pvEffectList->begin() ; it != new_bag_base.pvEffectList->end() ; ++it )
				{
					EffectInfo * pEffect = (*it);

					if( pEffect->eType != EffectInfo::EFFECT_TYPE_OPTIONAL )
						continue;

					if( pEffect->nMinLevel && GetLevel() < pEffect->nMinLevel ) continue;
					if( pEffect->nMaxLevel && GetLevel() > pEffect->nMaxLevel ) continue;

					if( pEffect->fValue[ 0 ] == ITEM_EFFECT_PASSIVE::CARRY_WEIGHT )		new_bag_capacity += pEffect->fValue[ 1 ];
					if( pEffect->fValue[ 3 ] == ITEM_EFFECT_PASSIVE::CARRY_WEIGHT )		new_bag_capacity += pEffect->fValue[ 4 ];
					if( pEffect->fValue[ 6 ] == ITEM_EFFECT_PASSIVE::CARRY_WEIGHT )		new_bag_capacity += pEffect->fValue[ 7 ];
					if( pEffect->fValue[ 9 ] == ITEM_EFFECT_PASSIVE::CARRY_WEIGHT )		new_bag_capacity += pEffect->fValue[ 10 ];
				}
			}

			current_bag_capacity *= m_fItemMod;
			new_bag_capacity *= m_fItemMod;

			if( current_bag_capacity != new_bag_capacity )
			{
				if( ( ( GetMaxWeight() >= GetWeight() ) && ( GetMaxWeight() - current_bag_capacity + new_bag_capacity < GetWeight() ) ) ||
					( ( GetMaxWeight() < GetWeight() ) && current_bag_capacity > new_bag_capacity ) )
				{
					return false;
				}
			}
		}
	}

	// 꾸미기 무기/방패일 경우 장착 위치에 대응하는 장비 위치에 현재 장착하고있는 아이템과 타입이 맞는지 체크
	if( pos == ItemBase::WEAR_DECO_WEAPON || pos == ItemBase::WEAR_DECO_SHIELD )
	{
		ItemBase::ItemWearType nRelativePos = ( pos == ItemBase::WEAR_DECO_WEAPON ) ? ItemBase::WEAR_WEAPON : ItemBase::WEAR_LEFTHAND;

		if( !m_anWear[ nRelativePos ] )
			return false;

		switch( m_anWear[ nRelativePos ]->GetItemClass() )
		{
		case ItemBase::CLASS_ONEHAND_SWORD:	if( pItem->GetItemClass() != ItemBase::CLASS_DECO_ONEHAND_SWORD	) return false; break;
		case ItemBase::CLASS_TWOHAND_SWORD:	if( pItem->GetItemClass() != ItemBase::CLASS_DECO_TWOHAND_SWORD	) return false; break;
		case ItemBase::CLASS_DAGGER:		if( pItem->GetItemClass() != ItemBase::CLASS_DECO_DAGGER		) return false; break;
		case ItemBase::CLASS_TWOHAND_SPEAR:	if( pItem->GetItemClass() != ItemBase::CLASS_DECO_TWOHAND_SPEAR	) return false; break;
		case ItemBase::CLASS_TWOHAND_AXE:	if( pItem->GetItemClass() != ItemBase::CLASS_DECO_TWOHAND_AXE	) return false; break;
		case ItemBase::CLASS_ONEHAND_MACE:	if( pItem->GetItemClass() != ItemBase::CLASS_DECO_ONEHAND_MACE	) return false; break;
		case ItemBase::CLASS_TWOHAND_MACE:	if( pItem->GetItemClass() != ItemBase::CLASS_DECO_TWOHAND_MACE	) return false; break;
		case ItemBase::CLASS_HEAVY_BOW:		if( pItem->GetItemClass() != ItemBase::CLASS_DECO_HEAVY_BOW		) return false; break;
		case ItemBase::CLASS_LIGHT_BOW:		if( pItem->GetItemClass() != ItemBase::CLASS_DECO_LIGHT_BOW		) return false; break;
		case ItemBase::CLASS_CROSSBOW:		if( pItem->GetItemClass() != ItemBase::CLASS_DECO_CROSSBOW		) return false; break;
		case ItemBase::CLASS_ONEHAND_STAFF:	if( pItem->GetItemClass() != ItemBase::CLASS_DECO_ONEHAND_STAFF	) return false; break;
		case ItemBase::CLASS_TWOHAND_STAFF:	if( pItem->GetItemClass() != ItemBase::CLASS_DECO_TWOHAND_STAFF	) return false; break;
		case ItemBase::CLASS_ONEHAND_AXE:	if( pItem->GetItemClass() != ItemBase::CLASS_DECO_ONEHAND_AXE	) return false; break;
		case ItemBase::CLASS_SHIELD:		if( pItem->GetItemClass() != ItemBase::CLASS_DECO_SHIELD		) return false; break;
		default:
			return false;
		}
	}

	if( pos >= ItemBase::MAX_ITEM_WEAR || pos < 0 )
	{
		return false;
	}

	if( vpOverlappItemList )
	{
		// 장착 위치에 무언가 입고 있다면 벗는다
		if( m_anWear[ pos ] )
		{
			vpOverlappItemList->push_back( pos );
		}

		// 예외처리 #1. 두손무기를 장착해야 하는데 이미 왼손에 무언가 들고 있다면 벗는다
		if( pItem->IsTwoHandItem() && m_anWear[ ItemBase::WEAR_LEFTHAND ] )
		{
			if( ( !pItem->IsBow() && !pItem->IsCrossBow() ) || !m_anWear[ ItemBase::WEAR_LEFTHAND ]->IsBullet() )
				vpOverlappItemList->push_back( ItemBase::WEAR_LEFTHAND );

			if( m_anWear[ ItemBase::WEAR_DECO_SHIELD ] )
			{
				vpOverlappItemList->push_back( ItemBase::WEAR_DECO_SHIELD );
			}
		}

		if( pos == ItemBase::WEAR_SHIELD )
		{
			if( pItem->GetItemClass() == ItemBase::CLASS_SHIELD && m_anWear[ ItemBase::WEAR_DECO_SHIELD ] && m_anWear[ ItemBase::WEAR_DECO_SHIELD ]->GetItemClass() != ItemBase::CLASS_DECO_SHIELD )
			{
				vpOverlappItemList->push_back( ItemBase::WEAR_DECO_SHIELD );
			}

			if( m_anWear[ ItemBase::WEAR_DECO_SHIELD ] && m_anWear[ ItemBase::WEAR_DECO_SHIELD ]->GetItemClass() == ItemBase::CLASS_DECO_SHIELD )
			{
				vpOverlappItemList->push_back( ItemBase::WEAR_DECO_SHIELD );
			}
		}

		// 예외처리 #2. 왼손에 장착해야 하는데 이미 두손무기를 들고 있다면 벗는다
		if( m_anWear[ ItemBase::WEAR_WEAPON ] &&
			m_anWear[ ItemBase::WEAR_WEAPON ]->IsTwoHandItem() &&
			pos == ItemBase::WEAR_LEFTHAND )
		{
			// exception #2-1. if player wear bow or crossbow, bullet wearing is accpted.
			if( ( !m_anWear[ ItemBase::WEAR_WEAPON ]->IsBow() && !m_anWear[ ItemBase::WEAR_WEAPON ]->IsCrossBow() ) || !pItem->IsBullet() )
			{
				vpOverlappItemList->push_back( ItemBase::WEAR_RIGHTHAND );
			}
		}

		// 예외처리 #3. 투슬롯 반지를 장착해야 하는데 이미 오른쪽에 반지를 입고 있으면 벗는다
		if( m_anWear[ ItemBase::WEAR_SECOND_RING ] &&
			pItem->GetWearType() == ItemBase::WEAR_TWOFINGER_RING )
		{
			vpOverlappItemList->push_back( ItemBase::WEAR_SECOND_RING );
		}

		if( m_anWear[ ItemBase::WEAR_RING ] &&
			m_anWear[ ItemBase::WEAR_RING ]->GetWearType() == ItemBase::WEAR_TWOFINGER_RING &&
			pItem->GetWearType() == ItemBase::WEAR_RING )
		{
			vpOverlappItemList->push_back( ItemBase::WEAR_RING );
		}

	}

	//emprs->nRaceLimit	= ItemBase::LIMIT_ASURA
	/*
	if( emprs->limit_asura )	emprs->nRaceLimit	|= ItemBase::LIMIT_ASURA;
	if( emprs->limit_gaia )		emprs->nRaceLimit	|= ItemBase::LIMIT_GAIA;
	if( emprs->limit_deva )		emprs->nRaceLimit	|= ItemBase::LIMIT_DEVA;

	if( emprs->limit_hunter )	emprs->nLimit	|= ItemBase::LIMIT_HUNTER;
	if( emprs->limit_fighter )	emprs->nLimit	|= ItemBase::LIMIT_FIGHTER;
	if( emprs->limit_magician ) emprs->nLimit	|= ItemBase::LIMIT_MAGICIAN;
	if( emprs->limit_summoner ) emprs->nJobLimit	|= ItemBase::LIMIT_SUMMONER;
	*/

	return true;
}

void StructPlayer::ProcEtherealDurabilityConsumption( const bool bIsAttacker, const DamageType eDamageType, const int nDamage )
{
	// 실제 소모량 = (기본 소모량) * (소모율) * (상황에 따른 소모율)

	// 기본 소모량
	const int nBaseConsumption = GameRule::GetEtherealDurabilityBaseConsumption( bIsAttacker, eDamageType );

	// 기본 소모량이 0이면 소모율과 상황에 다른 소모율이 몇이어도 소모되지 않음
	if( !nBaseConsumption )
		return;

	// 소모율(계산값)
	c_fixed10 fConsumeRate( GameRule::GetEtherealDurabilityConsumeRate( GetLevel(), GetJobId(), bIsAttacker, nDamage ) );

	// 상황에 따른 보정치(기본값 100%)
	c_fixed10 fEnvironmentalConsumeRate( 1 );

	// 상태에 따른 소모율 계산
	{
		// 대련 상대끼리나 데스매치 존, 배틀 아레나 안에서는 5%(대련 상대가 아닌 대상에게 피격당할 경우 대련 상태가 풀리므로 별도 체크 안 함)
		// 본래대로라면 대련이 시작된 이후, 즉 IsInStartedCompete() 함수로 체크해야 하나 매 공격마다 체크하기엔 부하가 있으므로 일단 예외로 둠
		if( GetCompeteID() || IsInDeathmatch() || IsInBattleArena() )
			fEnvironmentalConsumeRate = GameRule::ETHEREAL_DURABILITY_ENVIRONMENTAL_CONSUME_RATE_ON_PVP;
		// PK On 상태면 200%
		else if( IsPKOn() || IsPKOffing() )
			fEnvironmentalConsumeRate = GameRule::ETHEREAL_DURABILITY_ENVIRONMENTAL_CONSUME_RATE_ON_PK;
	}

	// 관련 아이템 선별 리스트 미리 선언
	static const ItemBase::ItemWearType aeRightHand[] =	{ ItemBase::WEAR_RIGHTHAND };
	static const ItemBase::ItemWearType aeLeftHand[] =	{ ItemBase::WEAR_LEFTHAND };
	static const ItemBase::ItemWearType aeArmors[] = { ItemBase::WEAR_SHIELD, ItemBase::WEAR_ARMOR, ItemBase::WEAR_HELM, ItemBase::WEAR_GLOVE,
		ItemBase::WEAR_BOOTS, ItemBase::WEAR_BELT, ItemBase::WEAR_MANTLE, ItemBase::WEAR_ARMULET, ItemBase::WEAR_RING, ItemBase::WEAR_SECOND_RING, ItemBase::WEAR_EAR };

	// 데미지 타입과 공격/피격 여부에 따라 내구도 관련 아이템 선별
	const ItemBase::ItemWearType * pItemPosList = NULL;
	int nPosListLength = 0;

	if( bIsAttacker )
	{
		// 공격 시에는 데미지 타입에 따라 
		switch( eDamageType )
		{
		case DT_NORMAL_PHYSICAL_DAMAGE:
		case DT_NORMAL_PHYSICAL_SKILL_DAMAGE:
		case DT_NORMAL_MAGICAL_DAMAGE:
		case DT_STATE_PHYSICAL_DAMAGE:
		case DT_STATE_MAGICAL_DAMAGE:
			// 일반 평타, 물리/마법 스킬 공격, 지속효과 데미지 처리 시 오른손만
			pItemPosList = aeRightHand;
			nPosListLength = _countof( aeRightHand );
			break;
		case DT_NORMAL_PHYSICAL_LEFT_HAND_DAMAGE:
			// 일반 평타 왼손 공격 시 왼손만(이미 무기가 착용되어 있다고 가정)
			pItemPosList = aeLeftHand;
			nPosListLength = _countof( aeLeftHand );
			break;
		}
	}
	else
	{
		// 피격 시에는 모든 방어구 착용 위치
		pItemPosList = aeArmors;
		nPosListLength = _countof( aeArmors );
	}

	if( !pItemPosList )
	{
		assert( 0 );
		return;
	}

	// 대상 아이템들 실제 에테리얼 내구도 감소 처리 적용
	bool bNeedToCalculateStat = false;
	for( int i = 0 ; i < nPosListLength ; ++i )
	{
		static __declspec( thread ) StructItem * pItemForDebugging;
		StructItem *pItem = GetWearedItem( pItemPosList[ i ] );

		pItemForDebugging = pItem;

		// 아이템이 없거나 공격 시인데 무기가 아니거나 피격 시인데 무기면 패스
		if( !pItem || bIsAttacker != pItem->IsWeapon() )
			continue;

		if( !pItem->ProcEtherealDurabilityConsumption( nBaseConsumption, fConsumeRate, fEnvironmentalConsumeRate ) )
			continue;

		SendItemMessage( this, pItem );

		// 새로 파괴된 아이템이 있으면 스텟 재 계산
		if( !pItem->GetCurrentEtherealDurability() )
		{
			LOG::Log11N4S( LM_ITEM_ETHEREAL_EXHAUST, 
				GetAccountID(), GetSID(), // N1, N2
				pItem->GetItemEnhance() * 100 + pItem->GetItemLevel(), pItem->GetItemCode(), // N3, N4
				0, 0, 0, pItem->GetMaxEtherealDurability(), // N5, N6, N7, N8
				GetX(), GetY(), pItem->GetItemUID(), // N9, N10, N11
				GetAccountName(), LOG::STR_NTS,
				GetName(), LOG::STR_NTS,
				"", 0,
				bIsAttacker?"ATTACKING":"BEING_ATTACKED", LOG::STR_NTS );
			bNeedToCalculateStat = true;
		}
	}

	// 피격 시에는 추가적으로 벨트 아이템의 내구도도 감소한다.
	if( !bIsAttacker )
	{
		for( int i = 0; i < m_nBeltSlotMax; ++i )
		{
			StructItem *pItem = m_aBeltSlotCard[i];

			if( !pItem || !pItem->IsEquipmentOnBelt() )
				continue;

			if( !pItem->ProcEtherealDurabilityConsumption( nBaseConsumption, fConsumeRate, fEnvironmentalConsumeRate ) )
				continue;

			SendItemMessage( this, pItem );

			// 새로 파괴된 아이템이 있으면 스텟 재 계산
			if( !pItem->GetCurrentEtherealDurability() )
			{
				LOG::Log11N4S( LM_ITEM_ETHEREAL_EXHAUST, 
					GetAccountID(), GetSID(), // N1, N2
					pItem->GetItemEnhance() * 100 + pItem->GetItemLevel(), pItem->GetItemCode(), // N3, N4
					0, 0, 0, pItem->GetMaxEtherealDurability(), // N5, N6, N7, N8
					GetX(), GetY(), pItem->GetItemUID(), // N9, N10, N11
					GetAccountName(), LOG::STR_NTS,
					GetName(), LOG::STR_NTS,
					"", 0,
					"BEING_ATTACKED", LOG::STR_NTS );
				bNeedToCalculateStat = true;
			}
		}
	}	

	// 스텟 재계산이 필요할 경우 재계산
	if( bNeedToCalculateStat )
		CalculateStat();
}

StructItem* StructPlayer::FindItem( ItemBase::ItemCode code ) const
{
	return m_Inventory.Find( code );
}

StructItem*	StructPlayer::FindItem( ItemUID uid ) const
{
	return m_Inventory.Find( uid );
}

void StructPlayer::FindItem( ItemBase::ItemCode code, std::vector< StructItem * > & vList )
{
	m_Inventory.Find( code, vList );
}

struct StructItem* StructPlayer::FindStorageItem( ItemBase::ItemCode code ) const
{
	return m_Storage.Find( code );
}

StructItem*	StructPlayer::FindEmptySummonCard( ItemBase::ItemCode code ) const
{
	return m_Inventory.Find( code, ItemInstance::ITEM_FLAG_SUMMON, false );
}

StructItem* StructPlayer::FindEmptyPetCage( ItemBase::ItemCode code ) const
{
	return m_Inventory.Find( code, ItemInstance::ITEM_FLAG_CONTAIN_PET, false );
}

StructItem* StructPlayer::FindTamingSummonCard( ItemBase::ItemCode code ) const
{
	return m_Inventory.Find( code, ItemInstance::ITEM_FLAG_TAMING, true );
}

StructItem* StructPlayer::FindFarmedSummonCard( ItemUID uid ) const
{
	for( int i = 0; i < GameRule::FARM_MAX_COUNT; ++i )
	{
		if( !m_vFarmedSummonInfo[i] ) continue;

		if( m_vFarmedSummonInfo[i]->item->GetItemUID() == uid )
			return m_vFarmedSummonInfo[i]->item;
	}

	return NULL;
}

bool StructPlayer::EraseBullet( const __int64 & count )
{
	StructItem * pBullet = GetWearedItem( ItemBase::WEAR_BULLET );

	if( !pBullet || !pBullet->IsBullet() )
		return false;

	// 화살통이면 실제로 소모 안 함
	if( pBullet->GetItemClass() == ItemBase::CLASS_QUIVER )
		return true;

	if( pBullet->GetCount() < count )
		return false;

	if( pBullet->GetCount() == count )
	{
		Putoff( ItemBase::WEAR_BULLET );
	}

	bool bRet = eraseItem( pBullet, count );

	return bRet;
}

bool StructPlayer::EraseItem_( StructItem *pItem, const __int64 & count, const char* function, const int line )
{
	if( !IsErasable( pItem ) )
		return false;

	if( pItem->GetCount() < count || count < 1 )
		return false;

	pItem->m_deleteFunction = function;
	pItem->m_deleteLine = line;

	bool bRet = eraseItem( pItem, count );

	return bRet;
}

bool StructPlayer::EraseItemFromStorage( struct StructItem *pItem, const __int64 & count )
{
	// 창고 안열었으면 KIN
	if( !IsUsingStorage() ) return false;

	// 창고 아템 아니면 KIN
	if( !pItem->IsInStorage() || !m_Storage.IsValid( pItem ) ) return false;

	// 내꺼 아니면 KIN
	if( pItem->GetAccountID() != GetAccountID() ) return false;

	// 갯수 모자라면 KIN
	if( pItem->GetCount() < count || count < 1 ) return false;

	// 나눠지는게 아니면 일부만 지울 수 없음
	if( !pItem->IsJoinable() && pItem->GetCount() != count )
		return false;

	if( m_Storage.Erase( pItem, count ) )
	{
		LOG::Log11N4S( LM_BANK,
			GetAccountID(), GetSID(),
			pItem->GetItemEnhance() * 100 + pItem->GetItemLevel(),
			pItem->GetItemCode(),
			count,
			pItem->GetCount(),
			0,
			GetGold().GetRawData(),
			0,
			GetStorageGold().GetRawData(),
			pItem->GetItemUID(),
			GetAccountName(), LOG::STR_NTS,
			GetName(), LOG::STR_NTS,
			"", 0,
			"STORAGE_ITEM_REMOVE", 0 );

		return true;
	}

	return false;
}

bool StructPlayer::eraseItem( StructItem *pItem, const __int64 & count )
{
	itemFileLog( pItem, "ERASE" );

	m_Inventory.Erase( pItem, count );

	return true;
}

struct StructItem* StructPlayer::popItem( struct StructItem *pItem, const __int64 & count, bool bSkipUpdateItemToDB )
{
	if( pItem->GetCount() < count || count <= 0 )
	{
		return NULL;
	}

	// 인벤에서 꺼냄
	StructItem *pNewItem = m_Inventory.Pop( pItem, count, bSkipUpdateItemToDB );	

	if( pNewItem )
		itemFileLog( pNewItem, "POP" );

	return pNewItem;
}

const bool StructPlayer::procAutoRecover()
{
	if( IsDead() )
		return false;

	// 완전히 소모되어 파괴될 자동 회복 아이템의 리스트를 별도로 보관했다가 벡터 순회 이후에 인벤토리에서 제거
	// 벡터 순회 중에 EraseItem 등을 호출할 경우 CalculateStat 함수가 호출되어 m_vAutoRecoverInfo 벡터가 리셋되어 it가 무효화됨
	std::vector< StructItem * > vExpiredAutoRecoverItem;

	for( std::vector< AUTO_RECOVER_INFO >::iterator it = m_vAutoRecoverInfo.begin() ; it != m_vAutoRecoverInfo.end() ; /* 루프에서 ++it 처리 */ )
	{
		// 회복 조건에 해당하지 않으면 건너 뛰기
		switch( (*it).m_eRecoverType )
		{
		case RECOVER_TYPE_HP:
			if( (*it).m_fCondition < ( c_fixed10( GetHP() * 100 ) / GetMaxHP() ) )
			{
				++it;
				continue;
			}
			break;
		case RECOVER_TYPE_MP:
			if( (*it).m_fCondition < ( c_fixed10( GetMP() * 100 ) / GetMaxMP() ) )
			{
				++it;
				continue;
			}
			break;
		default:
			assert( 0 );
			++it;
			continue;
		}

		GameObject::iterator itItem = StructItem::get( (*it).m_hItem );
		StructItem * pItem = static_cast< StructItem * >( *itItem );

		// 아이템이 없거나 자신의 인벤토리에 소유 중이 아니라면 해당 자동 회복 정보 제거
		if( !pItem || pItem->GetOwnerHandle() != GetHandle() )
		{
			it = m_vAutoRecoverInfo.erase( it );
			continue;
		}

		// 회복 계산 처리
		switch( (*it).m_eRecoverType )
		{
		case RECOVER_TYPE_HP:
			{
				int nRecover = ( (*it).m_fRecoverTo * GetMaxHP() / 100 ) - GetHP();

				// 회복 조건에는 해당되지만 실제로 회복시킬 일이 없는 경우(조건 값이 회복 목표량보다 높은 경우로 데이터 문제임. 기획팀에 수정 요청해야 함)
				if( nRecover <= 0 )
				{
					assert( 0 );
					++it;
					continue;
				}

				// 소모된 아이템의 수와 남은 수(중첩 아이템의 연속 소모 처리를 위해 사용됨)
				__int64 nUsedCount = 0;
				__int64 nRemainCount = pItem->GetCount();

				while( nRecover > 0 && nRemainCount > nUsedCount )
				{
					// 현재 아이템으로 회복 가능한 내구도
					int nAvailableRecoverAmount = pItem->GetCurrentEndurance();

					// 회복시켜야 할 양보다 남은 내구도가 적거나 같은 경우(딱 떨어지게 남은 내구도로 회복시킨 경우도 아이템 소모 처리 필요)
					int nPrevHP = GetHP();
					if( nRecover >= nAvailableRecoverAmount )
					{
						// 남아있던 내구도만큼만 회복시키고 소모 처리
						AddHP( nAvailableRecoverAmount );
						++nUsedCount;
						pItem->SetCurrentEndurance( ( nRemainCount > nUsedCount ) ? pItem->GetMaxEndurance() : 0 );

						LOG::Log11N4S( LM_ITEM_AUTO_RECOVER, GetAccountID(), GetSID(), RECOVER_TYPE_HP, nPrevHP, GetHP(), GetMaxHP(), nAvailableRecoverAmount, nAvailableRecoverAmount, 0, 0, pItem->GetItemUID(), GetAccountName(), LOG::STR_NTS, GetName(), LOG::STR_NTS, "", 0, "", 0 );

						vExpiredAutoRecoverItem.push_back( pItem );
						nRecover -= nAvailableRecoverAmount;
					}
					// 회복시켜야 할 양 이상으로 내구도가 남아있던 경우
					else
					{
						// 회복시켜야 할 양만큼 회복(아이템에 의한 회복 함수(HealByItem)를 사용하면 회복률에 영향을 받지만 이 회복 유형은 절대값으로 회복시켜야 함)
						AddHP( nRecover );
						pItem->SetCurrentEndurance( pItem->GetCurrentEndurance() - nRecover );

						LOG::Log11N4S( LM_ITEM_AUTO_RECOVER, GetAccountID(), GetSID(), RECOVER_TYPE_HP, nPrevHP, GetHP(), GetMaxHP(), nRecover, nAvailableRecoverAmount, pItem->GetCurrentEndurance(), 0, pItem->GetItemUID(), GetAccountName(), LOG::STR_NTS, GetName(), LOG::STR_NTS, "", 0, "", 0 );

						nRecover = 0;
					}

					SendItemMessage( this, pItem );
					BroadcastHPMPMsg( this, GetHP() - nPrevHP, 0 );
				}
			}
			break;
		case RECOVER_TYPE_MP:
			{
				int nRecover = ( (*it).m_fRecoverTo * GetMaxMP() / 100 ) - GetMP();

				// 회복 조건에는 해당되지만 실제로 회복시킬 일이 없는 경우(조건 값이 회복 목표량보다 높은 경우로 데이터 문제임. 기획팀에 수정 요청해야 함)
				if( nRecover <= 0 )
				{
					assert( 0 );
					++it;
					continue;
				}

				// 소모된 아이템의 수와 남은 수(중첩 아이템의 연속 소모 처리를 위해 사용됨)
				__int64 nUsedCount = 0;
				__int64 nRemainCount = pItem->GetCount();

				while( nRecover > 0 && nRemainCount > nUsedCount )
				{
					// 현재 아이템으로 회복 가능한 내구도
					int nAvailableRecoverAmount = pItem->GetCurrentEndurance();

					// 회복시켜야 할 양보다 남은 내구도가 적거나 같은 경우(딱 떨어지게 남은 내구도로 회복시킨 경우도 아이템 소모 처리 필요)
					int nPrevMP = GetMP();
					if( nRecover >= nAvailableRecoverAmount )
					{
						// 남아있던 내구도만큼만 회복(소모 예정 아이템은 내구도를 0으로 설정: 아이템 소모 처리에 문제 발생 시 부작용을 줄이기 위함)
						AddMP( nAvailableRecoverAmount );
						++nUsedCount;
						pItem->SetCurrentEndurance( ( nRemainCount > nUsedCount ) ? pItem->GetMaxEndurance() : 0 );

						LOG::Log11N4S( LM_ITEM_AUTO_RECOVER, GetAccountID(), GetSID(), RECOVER_TYPE_MP, nPrevMP, GetMP(), GetMaxMP(), nAvailableRecoverAmount, nAvailableRecoverAmount, 0, 0, pItem->GetItemUID(), GetAccountName(), LOG::STR_NTS, GetName(), LOG::STR_NTS, "", 0, "", 0 );

						// 아이템 소모 처리(여기서 직접 EraseItem을 호출하면 m_vAutoRecoverInfo 벡터가 리셋되어 문제 생김)
						vExpiredAutoRecoverItem.push_back( pItem );
						nRecover -= nAvailableRecoverAmount;
					}
					// 회복시켜야 할 양 이상으로 내구도가 남아있던 경우
					else
					{
						// 회복시켜야 할 양만큼 회복(아이템에 의한 회복 함수(MPHealByItem)를 사용하면 회복률에 영향을 받지만 이 회복 유형은 절대값으로 회복시켜야 함)
						AddMP( nRecover );
						pItem->SetCurrentEndurance( nAvailableRecoverAmount - nRecover );

						LOG::Log11N4S( LM_ITEM_AUTO_RECOVER, GetAccountID(), GetSID(), RECOVER_TYPE_MP, nPrevMP, GetMP(), GetMaxMP(), nRecover, nAvailableRecoverAmount, pItem->GetCurrentEndurance(), 0, pItem->GetItemUID(), GetAccountName(), LOG::STR_NTS, GetName(), LOG::STR_NTS, "", 0, "", 0 );

						nRecover = 0;
					}

					SendItemMessage( this, pItem );
					BroadcastHPMPMsg( this, 0, GetMP() - nPrevMP );
				}
			}
			break;
		default:
			assert( 0 );
			++it;
			continue;
		}

		++it;
	}

	// 실제 아이템 삭제 처리 오류 발생 시 유저 접속 끊기 위한 결과 체크
	bool bError = false;

	for( std::vector< StructItem * >::iterator it = vExpiredAutoRecoverItem.begin() ; it != vExpiredAutoRecoverItem.end() ; ++it )
	{
		StructItem * pItem = (*it);

		LOG::Log11N4S( LM_ITEM_USE, GetAccountID(), GetSID(), pItem->GetItemEnhance() * 100 + pItem->GetItemLevel(), pItem->GetItemCode(), 1, pItem->GetCount() - 1, 0, 0, 0, 0, pItem->GetItemUID(), GetAccountName(), LOG::STR_NTS, GetName(), LOG::STR_NTS, "", 0, "", 0 );

		if( !EraseItem( pItem, 1 ) )
		{
			assert( 0 );

			_cprint( "Auto recover item could not be erased. Account(%s)/Character(%s)/ItemSID(%I64d)/Count(%I64d)\n", GetAccountName(), GetName(), pItem->GetItemUID(), pItem->GetCount() );
			FILELOG( "Auto recover item could not be erased. Account(%s)/Character(%s)/ItemSID(%I64d)/Count(%I64d)", GetAccountName(), GetName(), pItem->GetItemUID(), pItem->GetCount() );

			bError = true;
		}
	}

	if( bError && pConnection && pConnection->IsConnected() )
	{
		pConnection->Close();
		return false;
	}

	return true;
}

void StructPlayer::onStandUp()
{
	RemoveStateIf( StateFlagChecker( StateInfo::ERASE_ON_STAND_UP ) );
}

void StructPlayer::ChangeItemPosition( bool is_storage, AR_HANDLE hItem_01, AR_HANDLE hItem_02 )
{
	if( is_storage && !IsUsingStorage() ) return;

	if( !hItem_01 || !hItem_02 ) return;

	StructItem * pItem_01 = StructItem::FindItem( hItem_01 );
	StructItem * pItem_02 = StructItem::FindItem( hItem_02 );

	if( !pItem_01 || !pItem_02 ) return;

	// 소유권 무결성 검사
	if( is_storage )
	{
		if( pItem_01->GetAccountID() != GetAccountID() ) return;
		if( pItem_02->GetAccountID() != GetAccountID() ) return;
	}
	else
	{
		if( pItem_01->GetOwnerHandle() != GetHandle() ) return;
		if( pItem_02->GetOwnerHandle() != GetHandle() ) return;
	}

	int nNewIdx = pItem_01->GetIdx();
	int nIdx = pItem_02->GetIdx();

	if( is_storage )
	{
		if( !m_Storage.Swap( nNewIdx, nIdx ) )
			return;
	}
	else
	{
		if( !m_Inventory.Swap( nNewIdx, nIdx ) )
			return;
	}

	SendItemMessage( this, pItem_01 );
	SendItemMessage( this, pItem_02 );

	// 이하 아이템 위치에 영향을 받는 정보 재정렬

	// 자동 회복 물약 성능 순서 재정렬
	for( std::vector< AUTO_RECOVER_INFO >::iterator it = m_vAutoRecoverInfo.begin() ; it != m_vAutoRecoverInfo.end() ; ++it )
	{
		if( (*it).m_hItem == hItem_01 )
			(*it).m_nOrderIndex = nNewIdx;
		else if( (*it).m_hItem == hItem_02 )
			(*it).m_nOrderIndex = nIdx;
	}
	std::sort( m_vAutoRecoverInfo.begin(), m_vAutoRecoverInfo.end(), _AutoRecoverInfoArrangeItemPositionPreceder );
}

unsigned short StructPlayer::ArrangeItem( const bool bIsStorage )
{
	// 창고 사용 중 모드 체크
	if( bIsStorage && !IsUsingStorage() )
	{
		assert( 0 );
		return RESULT_NOT_ACTABLE;
	}

	// 쿨타임 체크
	AR_TIME tCurrent = GetArTime();
	if( ( bIsStorage && m_nLastStorageArrangedTime + GameRule::ITEM_ARRANGE_COOL_TIME > tCurrent )
		|| ( !bIsStorage && m_nLastInvenArrangedTime + GameRule::ITEM_ARRANGE_COOL_TIME > tCurrent ) )
	{
		return RESULT_COOL_TIME;
	}

	if( bIsStorage )
	{
		m_Storage.Arrange();
		// 정렬 처리 시간 동안 흐른 시간을 쿨타임에서 제외하기 위해 시간을 다시 계산
		tCurrent = GetArTime();
		m_nLastStorageArrangedTime = tCurrent;
	}
	else
	{
		m_Inventory.Arrange();
		// 정렬 처리 시간 동안 흐른 시간을 쿨타임에서 제외하기 위해 시간을 다시 계산
		tCurrent = GetArTime();
		m_nLastInvenArrangedTime = tCurrent;

		// 이하 아이템 위치에 영향을 받는 정보 재정렬

		// 자동 회복 물약 성능 순서 재정렬
		for( std::vector< AUTO_RECOVER_INFO >::iterator it = m_vAutoRecoverInfo.begin() ; it != m_vAutoRecoverInfo.end() ; ++it )
		{
			GameObject::iterator itItem = StructItem::get( (*it).m_hItem );
			StructItem * pItem = static_cast< StructItem * >( *itItem );

			// 아이템이 없거나 자신의 인벤토리에 소유 중이 아니라면 맨 뒤로 빼기(이후 procAutoRecover에서 해당 정보 삭제 시 부하를 줄이기 위함)
			if( pItem && pItem->GetOwnerHandle() == GetHandle() )
				(*it).m_nOrderIndex = pItem->GetIdx();
			else
				(*it).m_nOrderIndex = INT_MAX;
		}
		std::sort( m_vAutoRecoverInfo.begin(), m_vAutoRecoverInfo.end(), _AutoRecoverInfoArrangeItemPositionPreceder );
	}

	return RESULT_SUCCESS;
}

// 아이템의 특성 변경으로 인해 인벤토리와 관련된 후 처리를 해야 할 필요가 있을 때 가급적 이 함수를 이용하도록 한다.
// 예를 들어, 강화 수치가 변경 된 중첩 가능한 아이템은 강화되고 난 후 인벤토리에 있는 아이템들과 합쳐질 가능성이 있다.
// 그럴 때 SetItemEnhance 함수의 호출만으로는 이러한 처리가 안되므로 이러한 후처리 함수를 만들어 그런 역할(중첩)을 하도록 한다.
// 또한 기존에 특성을 변경하고 나서 하는 후처리 (예: DB 세이브, ItemMessage 보내기 등)도 여기로 통일시킨다.
struct StructItem* StructPlayer::onAfterChangeItemProperty( struct StructItem *pItem )
{
	// Joinable 하지않거나, 주인 정보가 없는 아이템은 바로 저장하고 패스.
	if( !pItem->IsJoinable() || ( !pItem->IsInInventory() && !pItem->IsInStorage() ) )
	{
		SendItemMessage( this, pItem );
		if( pItem->IsNeedUpdateToDB() )
			pItem->DBQuery( new DB_UpdateItem( pItem ) );

		return pItem;
	}

	StructInventory *pContainer = &m_Inventory;
	if( pItem->IsInStorage() )
		pContainer = &m_Storage;

	StructItem *pExistItem = pContainer->FindJoinablePair( pItem );
	if( pExistItem )
	{
		if( pContainer->Pop( pItem, pItem->GetCount() ) )
		{
			StructItem *pJoinedItem = pContainer->Push( pItem, pItem->GetCount() );

			// 아이템이 합쳐지면 추적을 위해 로그 남겨준다.
			LOG::Log11N4S( LM_ITEM_JOIN, GetAccountID(), 
				GetSID(), pItem->GetItemEnhance() * 100 + pItem->GetItemLevel(), 
				pItem->GetItemCode(), pItem->GetCount(),
				pJoinedItem->GetCount(), GetGold().GetRawData(),
				0, 0,
				pJoinedItem->GetItemUID(), pItem->GetItemUID(),
				GetAccountName(), LOG::STR_NTS, 
				GetName(), LOG::STR_NTS,
				"", 0,
				"AFTER_CHANGE_PROPERTY", LOG::STR_NTS );
		}
	}
	else
	{
		SendItemMessage( this, pItem );
		if( pItem->IsNeedUpdateToDB() )
			pItem->DBQuery( new DB_UpdateItem( pItem ) );
	}

	return (pExistItem)? pExistItem : pItem;
}

struct StructItem* StructPlayer::DropItem( struct StructItem *pItem, const __int64 & count )
{
	// 버릴 위치 세팅
	ArPosition position = GetCurrentPosition( GetArTime() );
	pItem->SetCurrentXY( position.x, position.y );
	pItem->SetCurrentLayer( GetLayer() );

	if( !IsDropable( pItem ) )
	{
		return NULL;
	}
	StructItem *pNewItem = popItem( pItem, count );

	if( !pNewItem )
	{
		return NULL;		
	}

	__int64 nResultCount = 0;

	if( pItem != pNewItem )
	{
		// 아이템 쪼개 버릴 때 UID 발급 + DB Insert해줌. (추적을 위해)
		pNewItem->SetItemUID( allocItemUID() );
		pNewItem->DBQuery( new DB_InsertItem( pNewItem ) );
		nResultCount = pItem->GetCount();
	}

	// 로그 찍고...
	LOG::Log11N4S( LM_ITEM_DROP, GetAccountID(), GetSID(), pItem->GetItemEnhance() * 100 + pItem->GetItemLevel(), pItem->GetItemCode(), count, nResultCount, 0, pNewItem->GetItemUID(), position.x, position.y, pItem->GetItemUID(), GetAccountName(), LOG::STR_NTS, GetName(), LOG::STR_NTS, "", 0, "", 0 );

	// 월드에 등장시킴
	AddItemToWorld( pNewItem );

	return pNewItem;
}


bool StructPlayer::IsTakeable( AR_HANDLE ItemHandle, const __int64 & cnt )
{
	StructItem * pItem = StructItem::FindItem( ItemHandle );

	if( !pItem ) return false;

	// quest item
	if( pItem->IsQuestItem() && !IsTakeableQuestItem( pItem->GetItemCode() ) && !pItem->IsInStorage() ) return false;

	// 무게제한
	if( pItem->IsJoinable() && !!cnt )
	{
		if( pItem->GetWeight( cnt ) + m_fWeight > GetMaxWeight() ) return false;
	}
	else
	{
		if(	pItem->GetWeight() + m_fWeight	> GetMaxWeight() ) return false;
	}

	return true;
}

bool StructPlayer::GiveItem( struct StructPlayer *pTarget, AR_HANDLE ItemHandle, const __int64 & count, AR_HANDLE * pResultItemHandle )
{
	StructItem * pItem = StructItem::FindItem( ItemHandle );

	// 존재유무 검사
	if( !pItem )
		return false;

	if( !IsTradable( pItem ) )
		return false;

	// 갯수 충분한지 검사
	if( pItem->GetCount() < count )
	{
		return false;
	}

	itemFileLog( pItem, "GIVE" );

	// 전부 주는 것이라면..
	if( pItem->GetCount() == count )
	{
		StructItem *pDividedItem = popItem( pItem, count );

		// 인벤에서 아이템 꺼내는 거 실패
		if( !pDividedItem )
		{
			assert( 0 );
			return false;
		}

		pDividedItem->SetIdx( 0 );

		// 그리고 상대에게 준다.
		StructItem *pNewItem = pTarget->PushItem( pDividedItem, pDividedItem->GetCount() );

		if( pNewItem && pResultItemHandle )
		{
			*pResultItemHandle = pNewItem->GetHandle();
		}

		if( pNewItem && pNewItem != pDividedItem ) 
		{
			// _oprint( "Z:" );
			StructItem::PendFreeItem( pDividedItem );
		}
	}
	// 일부만 주는 것이라면..
	else if( pItem->IsJoinable() )
	{	
		// 일단 모든 속성 복제
		StructItem *pDividedItem = popItem( pItem, count );

		// 인벤에서 아이템 꺼내는 거 실패
		if( !pDividedItem )
		{
			assert( 0 );
			return false;
		}

		pDividedItem->SetIdx( 0 );

		// 상대에게 준다.
		StructItem *pNewItem = pTarget->PushItem( pDividedItem, pDividedItem->GetCount() );

		if( pNewItem && pResultItemHandle )
		{
			*pResultItemHandle = pNewItem->GetHandle();
		}

		if( pNewItem && pNewItem != pDividedItem ) 
		{
			// _oprint( "X:" );
			StructItem::PendFreeItem( pDividedItem );
		}
	}

	return true;
}

struct StructItem* StructPlayer::PushItem( struct StructItem *pItem, const __int64 & cnt, bool bSkipUpdateItemToDB )
{
	// 이미 내것이라면?
	if( pItem->GetOwnerUID() == GetPlayerUID() )
	{
		FILELOG( "StructPlayer::AddItem() :: owner is equal : %s : %I64d(%I64d)\n", GetName(), pItem->GetItemUID(), cnt );
		_cprint( "StructPlayer::AddItem() :: owner is equal : %s : %I64d(%I64d)\n", GetName(), pItem->GetItemUID(), cnt );

		assert( 0 );
		return NULL;
	}

	// 돈일경우
	if( pItem->IsGold() )
	{
		__int64 nPrevGoldAmount = GetGold().GetRawData();

		if( ChangeGold( GetGold() + pItem->GetCount() ) != RESULT_SUCCESS )
		{
			assert( 0 );
			_cprint( "ChangeGold/ChangeStorageGold Failed: Case[2], Player[%s], Info[Inven(%I64d), Add(%I64d), bSkipDBUpdate(%d)]\n", GetName(), nPrevGoldAmount, pItem->GetCount(), bSkipUpdateItemToDB );
			FILELOG( "ChangeGold/ChangeStorageGold Failed: Case[2], Player[%s], Info[Inven(%I64d), Add(%I64d), bSkipDBUpdate(%d)]", GetName(), nPrevGoldAmount, pItem->GetCount(), bSkipUpdateItemToDB );
		}
		// _oprint( "G:" );
		StructItem::PendFreeItem( pItem );
		return NULL;
	}

	// 인벤에 추가
	StructItem *pNewItem = m_Inventory.Push( pItem, cnt, bSkipUpdateItemToDB );

	// 디버그용
	if( pNewItem )
		itemFileLog( pNewItem, "PUSH" );

	return pNewItem;
}

struct StructItem* StructPlayer::PopItem( struct StructItem *pItem, const __int64 & cnt, bool bSkipUpdateItemToDB )
{
	if( !pItem || !cnt || pItem->GetCount() < cnt )
	{
		return NULL;
	}

	if( pItem->GetOwnerHandle() != GetHandle() || !pItem->IsInInventory() )
	{
		return NULL;
	}

	StructItem *pDividedItem = popItem( pItem, cnt, bSkipUpdateItemToDB );

	if( !pDividedItem )
	{
		return NULL;
	}

	pDividedItem->SetIdx( 0 );
	if( pDividedItem != pItem )
	{
		pDividedItem->SetOwnerInfo( NULL, 0, 0 );
		// 여기 들어왔다는 건 DB에 없는 아이템(나눠서 새로 생성된 아이템)이라는
		// 것이므로 DB에 Owner 정보 갱신이 필요 없음
	}

	return pDividedItem;
}

const size_t StructPlayer::GetItemCount()
{
	return m_Inventory.GetCount();
}

const size_t StructPlayer::GetStorageItemCount()
{
	return m_Storage.GetCount();
}

const __int64 StructPlayer::GetItemCount( ItemBase::ItemCode code )
{
	return m_Inventory.GetItemCount( code );
}

StructItem*	StructPlayer::GetItem( unsigned idx )
{
	return m_Inventory.Get( idx );
}

StructItem*	StructPlayer::GetStorageItem( unsigned idx )
{
	return m_Storage.Get( idx );
}

bool StructPlayer::MoveInventoryToStorage( StructItem *pItem, const __int64 & cnt )
{
	// 못 빼는 아템이면 KIN (이걸로 검사하는게 좀 이상하긴 한데.; 뭐 검사 루틴은 적절하니까. 나중에 나누든가 하자.)
	if( !IsErasable( pItem ) ) return false;

	if( pItem->GetBaseFlag().IsOn( ItemBase::FLAG_CANT_STORAGE ) )
		return false;

	// 창고로 넣는 경우에만 테이밍 중인 소환수 카드는 보관 불가
	if( pItem->GetInstanceFlag().IsOn( ItemInstance::ITEM_FLAG_TAMING ) ) return false;

	// 창고 안열었으면 KIN
	if( !IsUsingStorage() ) return false;

	// 갯수 모자라면 KIN
	if( pItem->GetCount() < cnt || cnt < 1 ) return false;

	// 나눠지는게 아니면 일부만 옮길 수 없음
	if( !pItem->IsJoinable() && pItem->GetCount() != cnt )
		return false;

	StructItem *pNewItem = NULL;

	// 단일 아이템
	if( !pItem->IsJoinable() ) 
	{
		// 인벤에서 제거
		if( !m_Inventory.Pop( pItem, cnt ) )
			return false;

		// 창고에 추가
		m_Storage.Push( pItem, cnt );

		LOG::Log11N4S( LM_BANK,
			GetAccountID(), GetSID(),
			pItem->GetItemEnhance() * 100 + pItem->GetItemLevel(),
			pItem->GetItemCode(),
			-1 * cnt,
			0,
			0,
			GetGold().GetRawData(),
			cnt,
			GetStorageGold().GetRawData(),
			pItem->GetItemUID(),
			GetAccountName(), LOG::STR_NTS,
			GetName(), LOG::STR_NTS,
			"", 0,
			"ITEM_INVENTORY_TO_STORAGE", LOG::STR_NTS );

		return true;
	}
	else	// 나눠빼기
	{
		__int64 nResultCnt = pItem->GetCount() - cnt;

		// 인벤에서 제거
		StructItem *pDividedItem = m_Inventory.Pop( pItem, cnt );
		if( !pDividedItem )
			return false;

		// 창고에 추가
		pNewItem = m_Storage.Push( pDividedItem, cnt );

		// 위에서 cnt 검사하고 들어와서 Push실패할일은 없지만 만일을 위해 추가
		if( pNewItem )
		{
			LOG::Log11N4S( LM_BANK,
				GetAccountID(), GetSID(),
				pItem->GetItemEnhance() * 100 + pItem->GetItemLevel(),
				pItem->GetItemCode(),
				0 - cnt,
				pItem->GetCount(),
				pItem->GetItemUID(),
				GetGold().GetRawData(),
				pNewItem->GetCount(),
				GetStorageGold().GetRawData(),
				pNewItem->GetItemUID(),
				GetAccountName(), LOG::STR_NTS,
				GetName(), LOG::STR_NTS,
				"", 0,
				"ITEM_INVENTORY_TO_STORAGE", LOG::STR_NTS );
		}

		if( pItem != pDividedItem )
		{
			// 기존 인벤의 아이템 갯수를 즉시 업데이트하자
			pItem->DBQuery( new DB_UpdateItem( pItem ) );
		}

		if( pNewItem != pDividedItem )
		{
			StructItem::PendFreeItem( pDividedItem );
		}

		return true;

	}

	return false;
}

bool StructPlayer::MoveStorageToInventory( StructItem *pItem, const __int64 & cnt )
{
	// 창고 안열었으면 KIN
	if( !IsUsingStorage() ) return false;

	// 창고 아템 아니면 KIN
	if( !pItem->IsInStorage() || !m_Storage.IsValid( pItem ) ) return false;

	// 내꺼 아니면 KIN
	if( pItem->GetAccountID() != GetAccountID() ) return false;

	// 갯수 모자라면 KIN
	if( pItem->GetCount() < cnt || cnt < 1 ) return false;

	// 나눠지는게 아니면 일부만 옮길 수 없음
	if( !pItem->IsJoinable() && pItem->GetCount() != cnt )
		return false;

	// 가질수 없으면 KIN
	if( !IsTakeable( pItem->GetHandle(), cnt ) ) return false;

	StructItem *pNewItem = NULL;


	// 단일 아이템
	if( !pItem->IsJoinable() ) 
	{		
		// 창고에서 제거
		if( !m_Storage.Pop( pItem, cnt ) )
			return false;

		// 인벤에 추가
		m_Inventory.Push( pItem, cnt );

		LOG::Log11N4S( LM_BANK,
			GetAccountID(), GetSID(),
			pItem->GetItemEnhance() * 100 + pItem->GetItemLevel(),
			pItem->GetItemCode(),
			cnt,
			cnt,
			0,
			GetGold().GetRawData(),
			0,
			GetStorageGold().GetRawData(),
			pItem->GetItemUID(),
			GetAccountName(), LOG::STR_NTS,
			GetName(), LOG::STR_NTS,
			"", 0,
			"ITEM_STORAGE_TO_INVENTORY", LOG::STR_NTS );

		return true;
	}
	else // 나눠 넣기
	{		
		__int64 nResultCnt = pItem->GetCount() - cnt;

		// 창고에서 제거
		StructItem *pDividedItem = m_Storage.Pop( pItem, cnt );
		if( !pDividedItem )
			return false;

		// 인벤에 추가
		pNewItem = m_Inventory.Push( pDividedItem, cnt );

		// 위에서 cnt 검사하고 들어와서 Push실패할일은 없지만 만일을 위해 추가
		if( pNewItem )
		{
			LOG::Log11N4S( LM_BANK,
				GetAccountID(), GetSID(),
				pItem->GetItemEnhance() * 100 + pItem->GetItemLevel(),
				pItem->GetItemCode(),
				cnt,
				pItem->GetCount(),
				pItem->GetItemUID(),
				GetGold().GetRawData(),
				pNewItem->GetCount(),
				GetStorageGold().GetRawData(),
				pNewItem->GetItemUID(),
				GetAccountName(), LOG::STR_NTS,
				GetName(), LOG::STR_NTS,
				"", 0,
				"ITEM_STORAGE_TO_INVENTORY", LOG::STR_NTS );
		}

		if( pItem != pDividedItem )
		{
			// 기존 인벤의 아이템 갯수를 즉시 업데이트하자
			pItem->DBQuery( new DB_UpdateItem( pItem ) );
		}

		if( pNewItem != pDividedItem )
		{
			StructItem::PendFreeItem( pDividedItem );
		}

		return true;
	}

	return false;
}




void	StructPlayer::ClearInventory()
{
	StructInventory *pInventory = GetInventory();

	if( pInventory == NULL )
	{
		FILELOG( "===== ClearInventory GetInventory() is NULL. pPlayer[%s] =====", GetName() );
		_cprint( "===== ClearInventory GetInventory() is NULL. pPlayer[%s] =====\n", GetName() );

		return;
	}

	size_t	nInventoryCount	=  pInventory->GetCount();
	size_t	nDeleteIndex	= 0;

	for( size_t i = 0; i < nInventoryCount; ++i )
	{
		StructItem *pItem = pInventory->Get( nDeleteIndex );

		if( pItem != NULL )
		{
			int nItemEnhance			= pItem->GetItemEnhance();
			int nItemLevel				= pItem->GetItemLevel();
			ItemBase::ItemCode nCode	= pItem->GetItemCode();
			__int64 nItemCount			= pItem->GetCount();
			ItemUID nItemUID			= pItem->GetItemUID();


			if( EraseItem( pItem, nItemCount ) == false )
			{
				nDeleteIndex = nDeleteIndex + 1;
			}
			else
			{
				LOG::Log11N4S( LM_ITEM_DELETE, GetAccountID(), GetSID(), nItemEnhance * 100 + nItemLevel, nCode, nItemCount, nItemCount, GetGold().GetRawData(), 0, GetX(), GetY(), nItemUID, GetAccountName(), LOG::STR_NTS, GetName(), LOG::STR_NTS, "SUCS", LOG::STR_NTS, "SCRIPT", LOG::STR_NTS );
			}
		}
	}

	return;
}





unsigned short StructPlayer::UseItem( struct StructItem *pItem, struct StructCreature *pTarget, const char *szParameter )
{
	if( !pTarget ) pTarget = this;

	// 갯수검사
	if( pItem->GetCount() < 1 ) return RESULT_ACCESS_DENIED;

	const ItemBaseServer & base = pItem->GetItemBase();

	unsigned short nRet = RESULT_SUCCESS;

	for( int x = 0; x < ItemBase::MAX_OPTION_NUMBER; ++x )
	{
		if( base.nBaseType[x] )
		{
			nRet = pTarget->onItemUseEffect( this, pItem, base.nBaseType[x], base.fBaseVar1[x], base.fBaseVar2[x], szParameter );
			if( nRet != RESULT_SUCCESS )
			{
				return nRet;
			}
		}

		if( base.nOptType[x] ) 
		{
			nRet = pTarget->onItemUseEffect( this, pItem, base.nOptType[x], base.fOptVar1[x], base.fOptVar2[x], szParameter );
			if( nRet != RESULT_SUCCESS )
			{
				return nRet;
			}
		}
	}

	if( base.pvEffectList && !base.pvEffectList->empty() )
	{
		for( std::vector< EffectInfo * >::const_iterator it = base.pvEffectList->begin() ; it != base.pvEffectList->end() ; ++it )
		{
			EffectInfo * pEffect = (*it);

			if( pEffect->eType != EffectInfo::EFFECT_TYPE_BASIC && pEffect->eType != EffectInfo::EFFECT_TYPE_OPTIONAL )
				continue;

			if( pEffect->nMinLevel && GetLevel() < pEffect->nMinLevel ) continue;
			if( pEffect->nMaxLevel && GetLevel() > pEffect->nMaxLevel ) continue;

			for( int i = 0 ; i < ItemBase::MAX_OPTION_NUMBER ; ++i )
			{
				const c_fixed10 & fOptEffectID = pEffect->fValue[ i * 3 ];

				if( !fOptEffectID )
					continue;

				nRet = pTarget->onItemUseEffect( this, pItem, fOptEffectID, pEffect->fValue[ i * 3 + 1 ], pEffect->fValue[ i * 3 + 2 ], szParameter );
				if( nRet != RESULT_SUCCESS )
				{
					return nRet;
				}
			}
		}
	}

	// 아이템 사용에 의한 스크립트 실행
	if( !pItem->GetItemBase().strScript.empty() )
	{
		std::string strScript( pItem->GetItemBase().strScript );

		const char * pszProcScript = "on_use_item";
		// 스크립트 내용에 on_use_item이 있을 경우 파라미터 추가
		if( strstr( strScript.c_str(), pszProcScript ) )
		{
			StructItem::_TARGET_TYPE eTargetType = StructItem::TARGET_TYPE_UNKNOWN;
			int nCode = 0;

			if( pTarget->IsPlayer() )
			{
				eTargetType = StructItem::TARGET_TYPE_PLAYER;
			}
			else if( pTarget->IsSummon() )
			{
				eTargetType = StructItem::TARGET_TYPE_SUMMON;
				nCode = static_cast< StructSummon * >( pTarget )->GetSummonCode();
			}
			else if( pTarget->IsMonster() )
			{
				eTargetType = StructItem::TARGET_TYPE_MONSTER;
				nCode = static_cast< StructMonster * >( pTarget )->GetMonsterId();
			}
			else if( pTarget->IsNPC() )
			{
				eTargetType = StructItem::TARGET_TYPE_NPC;
				nCode = static_cast< StructNPC * >( pTarget )->GetNPCID();
			}
			else
				assert( 0 );

			// on_use_item( 아이템 코드, 사용자 핸들, 타겟 타입, 타겟 핸들, NPC/Monster/Summon 코드값, [소환수인 경우에만]내껀지 남의껀지 )
			std::string strOnUseItem;
			XStringUtil::Format( strOnUseItem, "on_use_item( %d, %d, %d, %d, %d, %d )",
				pItem->GetItemCode(), GetHandle(), eTargetType, ( pTarget ) ? pTarget->GetHandle() : 0, nCode,
				( eTargetType == StructItem::TARGET_TYPE_SUMMON && static_cast< StructSummon * >( pTarget )->GetMaster() == this ) ? 1 : 0 );

			XStringUtil::Replace( strScript, pszProcScript, strlen( pszProcScript ), strOnUseItem.c_str(), strOnUseItem.length() );
		}

		// 일부 스크립트 함수에 의한 기능에서 대화 중인 NPC 핸들 값에 의해 제어되는 부분이 있으므로
		// 아이템에 의한 스크립트 실행 동안은 대화 중인 NPC 핸들을 0으로 설정함
		AR_HANDLE nContactNPCHandle = GetContactNPCHandle();
		SetContactNPCHandle( 0 );

		LUA()->RunString( strScript.c_str() );

		// 스크립트가 있으면서 사용형일 경우, 따로 TS_SC_RESULT를 보내주진 않는다. (스크립트 내에서 알아서 하라고)
		if( pItem->GetItemType() == ItemBase::TYPE_USE )
		{
			nRet = RESULT_SUCCESS_WITHOUT_NOTICE;
		}

		// 대화 중인 NPC 핸들 값을 원래대로 되돌림
		SetContactNPCHandle( nContactNPCHandle );
	}

	// 쿨타임 세팅
	if( pItem->GetCoolTimeGroup() ) 
	{
		AR_TIME t = GetArTime();
		m_nItemCoolTime[ pItem->GetCoolTimeGroup() - 1 ] = t + pItem->GetCoolTime();

		SendItemCoolTimeInfo( this );
	}

	// 귀환(캐쉬템)/귀환(레벨달성이벤트상품)/복귀의 깃털은 아이템 소모를 캐스팅 완료 후에 함.(여기선 그냥 성공으로 리턴)
	switch( pItem->GetItemCode() )
	{
	case ItemBase::ITEM_CODE_RETURN_FEATHER:
	case ItemBase::ITEM_CODE_RETURN_FEATHER_SECRET_DUNGEON:
	case ItemBase::ITEM_CODE_RETURN_BACK_FEATHER:
	case ItemBase::ITEM_CODE_EVENT_RETURN_FEATHER:
	case ItemBase::ITEM_CODE_RUPPI_RETURN_FEATHER:
	case ItemBase::ITEM_CODE_EVENT_RETURN_FEATHER2:
	case ItemBase::ITEM_CODE_RENAME_CHARACTER_TYPE1:
	case ItemBase::ITEM_CODE_RENAME_CHARACTER_TYPE2:
	case ItemBase::ITEM_CODE_RENAME_CHARACTER_TYPE3:
	case ItemBase::ITEM_CODE_RENAME_CHARACTER_TYPE4:
	case ItemBase::ITEM_CODE_CREATURE_TAMING_SCROLL:
	case ItemBase::ITEM_CODE_CREATURE_TAMING_SCROLL_EVENT:
	case ItemBase::ITEM_CODE_CREATURE_TAMING_SCROLL_EVENT2:
		return nRet;

	default:
		break;
	}

	// 호칭
	UpdateTitleConditionByItemUse( pItem->GetItemCode() );

	bool bIsCashItem = pItem->IsCashItem();

	if( pItem->GetItemBase().nType != ItemBase::TYPE_USE )
		EraseItem( pItem, 1 );			// 갯수 소모

	// 테섭일경우 캐쉬템 써도 저장 안 함
	if( bIsCashItem && ENV().GetInt( "game.ServiceServer" ) )
		Save();

	return nRet;
}

int StructPlayer::GetMoveSpeed() const
{
	int nMoveSpeed = StructCreature::GetMoveSpeed();
	if( IsWalking() )
		nMoveSpeed >>= 1;	// 나누기 2

	// { 아템 소지량에 따른 이속 저하
	float fWT = (float) GetWeight() / GetMaxWeight();
	if( fWT >= 1.0f || fWT < 0 ) return nMoveSpeed * 0.1f;
	else if( fWT >= 0.75f ) return nMoveSpeed * 0.5f;
	// }

	return StructCreature::GetMoveSpeed();
}

void StructPlayer::OnUpdate()
{
	ThreadPlayerHelper TPHelper( this );

	AR_TIME t = GetArTime();

	m_bIsGaiaMember = false;

	if( m_nGaiaValidTime > t ) m_bIsGaiaMember = true;

	if( m_nChatPenalty > 0 && m_nNextChatPenaltyDecreaseTime < t )
	{
		--m_nChatPenalty;
		m_nNextChatPenaltyDecreaseTime += 3000;
	}

	// m_bIsGaiaMember 세팅이 완료된 후에 호출되어야 함. 2배 보너스 적용.
	int stamina_regen = GetStaminaRegenRate();

	if( m_nLastStaminaUpdateTime + GameRule::STAMINA_UPDATE_TIME < t )
	{
		int update_tick = ( t - m_nLastStaminaUpdateTime ) / GameRule::STAMINA_UPDATE_TIME;
		m_nLastStaminaUpdateTime += update_tick * GameRule::STAMINA_UPDATE_TIME;

		AddStamina( update_tick * stamina_regen );
	}

	if( m_nLastBindSummonUpdateTime + GameRule::BIND_SUMMON_UPDATE_TIME < t )
	{
		m_nLastBindSummonUpdateTime = t;

		int nCreatureControlLevel = GetCurrentPassiveSkillLevel( StructSkill::SKILL_CREATURE_CONTROL );

		if( nCreatureControlLevel > 6 ) nCreatureControlLevel = 6;

		for( int i = 0; i < nCreatureControlLevel; ++i )
		{
			StructItem *pBindedCard = GetSummonCardAt( i );

			if( pBindedCard && pBindedCard->GetSummonStruct() )
			{
				StructSummon * pSummon = pBindedCard->GetSummonStruct();

				if( pSummon && !pSummon->IsInWorld() )
				{
					pSummon->OnUpdate();
				}
			}

		}
	}

	if( m_nTurnOffPkModeTime && m_nTurnOffPkModeTime < t )
	{
		LOG::Log11N4S( LM_CHARACTER_PK_MODE, GetAccountID(), GetSID(), 0, 0, 0, GetImmoralPoint(), 0, 0, 0, 0, 0, GetAccountName(), LOG::STR_NTS, GetName(), LOG::STR_NTS, "", 0, "OFF", LOG::STR_NTS );

		SetPKOff();

		m_nTurnOffPkModeTime = 0;

		BroadcastStatusMessage( this );
	}

	if( m_nTurnOnPkModeTime && m_nTurnOnPkModeTime < t )
	{
		float fIP = 5;

		if( GameRule::bIsPKServer )
		{
			fIP = 0.0f;
		}

		SetImmoralPoint( GetImmoralPoint() + fIP );

		LOG::Log11N4S( LM_CHARACTER_PK_MODE, GetAccountID(), GetSID(), 0, 0, fIP, GetImmoralPoint(), 0, 0, 0, 0, 0, GetAccountName(), LOG::STR_NTS, GetName(), LOG::STR_NTS, "", 0, "ON", LOG::STR_NTS );

		SetPKOn();

		m_nTurnOnPkModeTime = 0;

		BroadcastStatusMessage( this );
	}

	if( GetSubSummon() && !m_bIsInfiniteSummonTime && m_nNextUnSummonTime < t )
	{
		PendUnSummon( GetSubSummon() );
		ArcadiaServer::Instance().SetObjectPriority( this, ArSchedulerObject::UPDATE_PRIORITY_HIGHEST );
	}

	// 합계 접속 시간(중국 게임 중독 방지 시스템)은 30초마다 재계산
	AR_TIME nLastContinuousPlayTimeProcTime = GetLastContinuousPlayTimeProcTime();
	if( IsGameTimeLimited() && nLastContinuousPlayTimeProcTime + 3000 < t )
	{
		SetContinuousPlayTime( GetContinuousPlayTime() + ( t - nLastContinuousPlayTimeProcTime ) );
		SetLastContinuousPlayTimeProcTime( t );
	}

	if( m_nLastSaveTime + 30000 < t )
	{
		Save();

		ArPosition pos = GetCurrentPosition( t );
		ChangeLocation( pos.x, pos.y );
	}

	if( m_nLastUpdateTime + 100 < t )
	{
		// 이걸 변수화해서 따로 함수 2개를 호출하지 않고 if문 안에 넣으면 Short-cut evaluation에 의해 뒤쪽 함수가 실행되지 않을 수도 있음...-_ -;
		// 컴파일러 특성따라 함수 모두 실행해놓고 받은 결과를 최종적으로 if문에서 평가할 수도 있고 하나씩 실행해서 평가할 수도 있겠지만
		// 위험성을 가지고 있는 코드를 굳이 써야할 필요가 없으므로 패스.
		bool bExistClearedItem = ClearExpiredItem();
		bExistClearedItem = ClearExpiredElementalEffect() || bExistClearedItem;

		if( bExistClearedItem )
		{
			CalculateStat();
		}
	}

	// 플레이 포인트 관련
	if( GameRule::bUsePlayPoint )
	{
		int nAccumulatedPlayTime = GetPlayTime();
		int nPlayTimeInMin = ( t - GetLastPlayTimeUpdateTime() ) / ( 100 * 60 );

		// 플레이 시간 단위가 변했으면 업데이트
		int nPoint = ( ( ( nAccumulatedPlayTime + nPlayTimeInMin ) / GameRule::nPlayPointAccumulateTerm ) - ( nAccumulatedPlayTime / GameRule::nPlayPointAccumulateTerm ) ) * GameRule::nPlayPointAccumulateAmount;
		if( nPoint )
		{
			if( GetPCBangMode() == GameRule::PCBANG_PREMIUM_BONUS )
			{
				nPoint *= GameRule::fPremiumPCBangPlayPointBonusRate;
			}

			DBQuery( new DB_UpdatePlayTimePoint( this, nPlayTimeInMin, nPoint ) );

			SetPlayTime( nAccumulatedPlayTime + nPlayTimeInMin );
			SetLastPlayTimeUpdateTime( t );

			std::string szString;
			XStringUtil::Format( szString, "@441\v#@point@#\v%d", nPoint );
			SendChatMessage( false, CHAT_NOTICE, "@NOTICE", this, szString.c_str() );

			LOG::Log11N4S( LM_PLAY_POINT, GetAccountID(), GetSID(),
				GetPlayTime(), nPlayTimeInMin,
				nPoint, GetPCBangMode(),
				0, 0, 0, 0, 0,
				GetAccountName(), LOG::STR_NTS, GetName(), LOG::STR_NTS,
				"", LOG::STR_NTS, "", LOG::STR_NTS );
		}
	}

	// 스크립트 기반 시간제 이벤트를 사용 한다면
	if( GameRule::bUseTimeBasedEventScript )
	{
		int nTimeForRewardInMin = ( t - GetLastTimeBasedEventTimeScript() ) / ( 100 * 60 );

		// 최소 1분마다 발생
		if( GameRule::nTermForTimeBasedEventScript < 1 )
			GameRule::nTermForTimeBasedEventScript = 1;
		int nRewardCount = nTimeForRewardInMin / GameRule::nTermForTimeBasedEventScript;

		// 부득이하게 서버가 극단적으로 늦게 돌고 타임 기반 이벤트 보상주기가 짧은 경우 
		// 가끔 RewardCount가 1보다 클 수 있음. 이 경우 그 만큼 보상스크립트를 호출한다. 
		// (예: Update가 2분만에 이루어졌는데 보상 주기가 1분인경우)
		for( int i = 0 ; i < nRewardCount ; ++i )
		{
			ARCADIA_LOCK( ArcadiaServer::Instance().LockObjectWithVisibleRange( this ) );

			// 지역락을 걸고 스크립트를 호출하므로 스크립트 안에서 데드락을 유발하는 스크립트를
			// 호출하면 안된다. (add_event_state같은..)
			LUA()->RunString( "on_time_based_event_reward()" );
			
			LOG::Log11N4S( LM_TIME_BASED_EVENT_REWARD, GetAccountID(), GetSID(),
				nTimeForRewardInMin, nRewardCount,
				GetLastTimeBasedEventTimeScript(), 0, 0, 0, 0, 0, 0,
				GetAccountName(), LOG::STR_NTS, GetName(), LOG::STR_NTS,
				"", LOG::STR_NTS, "SCRIPT", LOG::STR_NTS );

			SetLastTimeBasedEventTimeScript( GetLastTimeBasedEventTimeScript() + GameRule::nTermForTimeBasedEventScript * 100 * 60 );
		}
	}

	// DB SP 기반 시간제 이벤트를 사용 한다면
	if( GameRule::bUseTimeBasedEventDB )
	{
		int nTimeForRewardInMin = ( t - GetLastTimeBasedEventTimeDB() ) / ( 100 * 60 );

		// 최소 1분마다 발생
		if( GameRule::nTermForTimeBasedEventDB < 1 )
			GameRule::nTermForTimeBasedEventDB = 1;
		int nRewardCount = nTimeForRewardInMin / GameRule::nTermForTimeBasedEventDB;

		// 부득이하게 서버가 극단적으로 늦게 돌고 타임 기반 이벤트 보상주기가 짧은 경우 
		// 가끔 RewardCount가 1보다 클 수 있음. 이 경우 그 만큼 보상스크립트를 호출한다. 
		// (예: Update가 2분만에 이루어졌는데 보상 주기가 1분인경우)
		for( int i = 0 ; i < nRewardCount ; ++i )
		{
			// DB SP 호출
			DBQuery( new DB_UpdateTimeBasedEvent( this ) );

			LOG::Log11N4S( LM_TIME_BASED_EVENT_REWARD, GetAccountID(), GetSID(),
				nTimeForRewardInMin, nRewardCount,
				GetLastTimeBasedEventTimeDB(), 0, 0, 0, 0, 0, 0,
				GetAccountName(), LOG::STR_NTS, GetName(), LOG::STR_NTS,
				"", LOG::STR_NTS, "DB", LOG::STR_NTS );

			SetLastTimeBasedEventTimeDB( GetLastTimeBasedEventTimeDB() + GameRule::nTermForTimeBasedEventDB * 100 * 60 );
		}
	}

	// 파티원 정보 갱신
	if( IsInParty() )
	{
		SendPartyPosition( this );
	}

	// 시간제 퀘스트 체크
	m_QuestManager.ProcExpiredQuest( t );

	StructCreature::OnUpdate();

	// 자동 회복 아이템에 의한 회복 처리
	procAutoRecover();
	
	time_t currentTime = time(NULL);

	currentTime = (currentTime + time_test) / 60;

	if (currentTime != m_nLastMin)
	{
		char buff[256];

		int minMod = currentTime % 60;
		this->m_nLastMin = currentTime;
		ArPosition currentPos = GetCurrentPosition( t );

		if (currentPos.x >= 161280.0 && currentPos.x <= 177408.0 && currentPos.y >= 145152.0 && currentPos.y <= 161280.0)
		{
			s_sprintf(buff, 255, "on_forgotten_island(%d)", minMod);
			LuaVM::Inst()->RunString(buff);
		}
		else if (currentPos.x >= 155211.0 && currentPos.x <= 155483.0 && currentPos.y >= 149451.0 && currentPos.y <= 149574.0)
		{
			s_sprintf(buff, 255, "on_forgotten_ship(%d)", minMod);
			LuaVM::Inst()->RunString(buff);
		}
		else if (currentPos.x >= 154837.0 && currentPos.x <= 156123.0 && currentPos.y >= 149385.0 && currentPos.y <= 150359.0)
		{
			s_sprintf(buff, 255, "on_forgotten_port(%d)", minMod);
			LuaVM::Inst()->RunString(buff);
		}
		else if ( (currentPos.x  <= 128394 && currentPos.y <= 112407) || (currentPos.x <= 127810 && currentPos.y  <= 109010))
		{
			//s_sprintf(buff, 255, "on_siege_end(%d)", minMod);
			//LuaVM::Inst()->RunString(buff);
		}
	}
}

void StructPlayer::onExpChange()
{
	int nLevel = 1;

	while( GameContent::GetNeedExp( nLevel ) <= m_nEXP )
	{
		if( nLevel >= GameRule::MAX_LEVEL ) break;
		++nLevel;
	}

	SendExpMsg( this, this );
	sendBonusExpJpMsg();

	if( nLevel && nLevel != m_nLevel )
	{
		LOG::Log11N4S( LM_CHARACTER_LEVEL_UP, GetAccountID(), GetSID(), 0, m_nLevel, nLevel, m_nMaxReachedLevel, 0, 0, 0, 0, 0, GetAccountName(), LOG::STR_NTS, GetName(), LOG::STR_NTS, "", 0, "", 0 );

		// 렙업 핸들러 호출
		if( nLevel > m_nLevel )
		{
			m_nLevel = nLevel;

			ThreadPlayerHelper TPHelper( this );

			LUA()->RunString( "on_player_level_up()" );

			// 위의 스크립트 함수(on_player_level_up) 안에서 m_nLevel, m_nMaxReachedLevel 을 참조하므로 m_nMaxReachedLevel 은 위의 스크립트 함수 실행 이후에 갱신해야 함
			if( m_nLevel > m_nMaxReachedLevel )
				m_nMaxReachedLevel = m_nLevel;

			// 레벨 때문에 안 보이던 새 퀘스트가 보일 수도 있기 때문.
			SendNPCStatusInVisibleRange( this );

			int prev_hp = GetHP();
			int prev_mp = GetMP();

			CalculateStat();

			if( !IsDead() )
			{
				m_nHP = m_nMaxHP;
				m_nMP = m_nMaxMP;
			}

			BroadcastHPMPMsg( this, GetHP() - m_nHP, GetMP() - m_nMP );
		}
		else
		{
			m_nLevel = nLevel;

			CalculateStat();
		}

		// 길드 정보 갱신
		if( IsInGuild() )
			// 레벨만 바뀌었으니 레벨 변경 메시지만 보내자(길드 정보에 보관된 정보도 갱신할 겸)
			//BroadcastGuildMemberInfo( GetGuildID(), this );
			GuildManager::GetInstance().OnChangeCharacterLevel( GetGuildID(), GetPlayerUID(), GetLevel() );

		// 파티 정보 갱신
		if( IsInParty() )
			// 레벨만 바뀌었으니 레벨 변경 메시지만 보내자(파티 정보에 보관된 정보도 갱신할 겸)
			//BroadcastPartyMemberInfo( GetPartyID(), this );
			PartyManager::GetInstance().OnChangeCharacterLevel( GetGuildID(), GetPlayerUID(), GetLevel() );

		setSummonUpdate();

		Save();
		BroadcastLevelMsg( this );
		//SendLevelMsg( this, this );
	}
	else
	{
		if( m_nLastExpSaveTime + 3000 < GetArTime() )
			Save( true );
	}
}

struct StructSummon*	StructPlayer::GetSummon( int summon_sid )
{
	std::vector< struct StructSummon* >::iterator	it;

	for( it = m_vSummonList.begin(); it != m_vSummonList.end(); ++it )
	{
		if( (*it)->GetSummonSID() == summon_sid ) return *it;
	}

	return NULL;
}

struct StructSummon*	StructPlayer::GetSummon( AR_HANDLE handle )
{
	std::vector< struct StructSummon* >::iterator	it;

	for( it = m_vSummonList.begin(); it != m_vSummonList.end(); ++it )
	{
		if( (*it)->GetHandle() == handle ) return *it;
	}

	return NULL;
}

void StructPlayer::AddSummon( struct StructSummon* pSummon, bool bSkipUpdateItemToDB, bool bSendMsg )
{
	pSummon->SetMaster( this );

	m_vSummonList.push_back( pSummon );
	if( bSendMsg ) SendAddSummonMessage( this, pSummon );

	if( !bSkipUpdateItemToDB && pSummon->IsLoginComplete() )
		pSummon->DBQuery( new DB_UpdateSummon( pSummon ) );
}

void StructPlayer::RemoveSummon( struct StructSummon* pSummon, bool bSkipUpdateItemToDB )
{
	std::vector< struct StructSummon* >::iterator	it;

	for( it = m_vSummonList.begin(); it != m_vSummonList.end(); ++it )
	{
		if( *it != pSummon ) continue;

		m_vSummonList.erase( it );
		break;
	}

	pSummon->SetMaster( NULL );

	SendRemoveSummonMessage( this, pSummon );

	if( !bSkipUpdateItemToDB )
		pSummon->DBQuery( new DB_UpdateSummon( pSummon ) );
}

void StructPlayer::AddSummonToStorage( struct StructSummon* pSummon, bool bSkipUpdateItemToDB )
{
	pSummon->SetAccountId( GetAccountID() );

	m_vStorageSummonList.push_back( pSummon );

	if( !bSkipUpdateItemToDB && pSummon->IsLoginComplete() )
		pSummon->DBQuery( new DB_UpdateSummon( pSummon ) );
}

void StructPlayer::RemoveSummonFromStorage( struct StructSummon* pSummon, bool bSkipUpdateItemToDB )
{
	std::vector< struct StructSummon* >::iterator	it;

	for( it = m_vStorageSummonList.begin(); it != m_vStorageSummonList.end(); ++it )
	{
		if( *it != pSummon ) continue;

		m_vStorageSummonList.erase( it );
		break;
	}

	pSummon->SetAccountId( 0 );

	if( !bSkipUpdateItemToDB )
		pSummon->DBQuery( new DB_UpdateSummon( pSummon ) );
}

void StructPlayer::AddPet( struct StructPet* pPet, bool bSkipUpdateItemToDB, bool bSendMsg )
{
	THREAD_SYNCHRONIZE( m_csPet );

#ifdef _DEBUG
	// 디버그모드일 때만 등록되어 있는 펫이 다시 등록되는지 검사
	for( std::vector< struct StructPet * >::iterator it = m_vPetList.begin() ; it != m_vPetList.end() ; ++it )
	{
		if( (*it) == pPet )
		{
			assert( 0 );
			break;
		}
	}
#endif

	m_vPetList.push_back( pPet );

	pPet->SetMaster( this );

	if( bSendMsg ) SendAddPetMessage( this, pPet );

	if( !bSkipUpdateItemToDB && pPet->IsLoginComplete() )
		pPet->DBQuery( new DB_UpdatePet( pPet ) );
}

void StructPlayer::RemovePet( struct StructPet* pPet, bool bSkipUpdateItemToDB )
{
	THREAD_SYNCHRONIZE( m_csPet );

	bool bErased = false;
	for( std::vector< struct StructPet * >::iterator it = m_vPetList.begin() ; it != m_vPetList.end() ; ++it )
	{
		if( (*it) != pPet ) continue;

		bErased = true;
		m_vPetList.erase( it );
		break;
	}

	assert( bErased );

	pPet->SetMaster( NULL );

	SendRemovePetMessage( this, pPet );

	if( !bSkipUpdateItemToDB )
		pPet->DBQuery( new DB_UpdatePet( pPet ) );
}

void StructPlayer::AddPetToStorage( struct StructPet* pPet, bool bSkipUpdateItemToDB )
{
	pPet->SetMaster( NULL );

	m_vStoragePetList.push_back( pPet );

	if( !bSkipUpdateItemToDB && pPet->IsLoginComplete() )
		pPet->DBQuery( new DB_UpdatePet( pPet ) );
}

void StructPlayer::RemovePetFromStorage( struct StructPet* pPet, bool bSkipUpdateItemToDB )
{
	std::vector< struct StructPet* >::iterator	it;

	for( it = m_vStoragePetList.begin(); it != m_vStoragePetList.end(); ++it )
	{
		if( *it != pPet ) continue;

		m_vStoragePetList.erase( it );
		break;
	}

	pPet->SetMaster( NULL );

	if( !bSkipUpdateItemToDB )
		pPet->DBQuery( new DB_UpdatePet( pPet ) );
}

unsigned short StructPlayer::SummonPet( struct StructPet* pPet )
{
	bool bIsOwnedPet = false;

	{
		THREAD_SYNCHRONIZE( m_csPet );

		for( std::vector< struct StructPet * >::iterator it = m_vPetList.begin() ; it != m_vPetList.end() ; ++it )
		{
			if( (*it) == pPet )
			{
				bIsOwnedPet = true;
				break;
			}
		}
	}

	if( !bIsOwnedPet || !pPet->GetParentCage() || pPet->GetParentCage()->GetOwnerHandle() != GetHandle() )
		return RESULT_NOT_OWN;

	if( pPet->IsInWorld() )
		return RESULT_ACCESS_DENIED;

	// 소환된 펫이 있으면 불가능
	if( m_pSummonedPet )
	{
		// 소환할 펫이 이미 소환된 펫이면 함수 호출이 잘못 들어오신 것(호출부에서 GetSummonedPet으로 검사하실 것)
		assert( m_pSummonedPet != pPet );
		if( m_pSummonedPet == pPet )
			return RESULT_ALREADY_EXIST;
		else
			return RESULT_ACCESS_DENIED;
	}

	ArPosition pos = GetPos();
	ArPosition valid_pos;
	if( GameContent::GetValidRandomPos( pos, valid_pos, StructPet::PET_SPAWN_MIN_LENGTH ) == false )
	{
		// 실패하면 주인 위치에...
		valid_pos = pos;
	}

	pPet->SetCurrentXY( valid_pos.x, valid_pos.y );
	pPet->SetCurrentLayer( GetLayer() );
	pPet->StopMove();

	AddPetToWorld( pPet );
	ArcadiaServer::Instance().SetObjectPriority( pPet, ArSchedulerObject::UPDATE_PRIORITY_NORMAL );

	m_pSummonedPet = pPet;

	// 펫의 성능 추가로 인해 소환된 펫을 부적처럼 사용하는 것이 가능해졌으므로 스탯을 재계산해야 한다.
	CalculateStat();

	return RESULT_SUCCESS;
}

bool StructPlayer::UnSummonPet()
{
	// 이미 역소환 시도가 되었던 펫인지 확인
	std::vector< StructPet * >::iterator itEnd = m_vPendingUnSummonPetList.end();
	if( std::find( m_vPendingUnSummonPetList.begin(), itEnd, m_pSummonedPet ) != itEnd )
	{
		assert( 0 );
		return true;
	}

	m_vPendingUnSummonPetList.push_back( m_pSummonedPet );
	m_pSummonedPet = NULL;

	// 펫의 성능 추가로 인해 소환된 펫을 부적처럼 사용하는 것이 가능해졌으므로 스탯을 재계산해야 한다.
	CalculateStat();

	return true;
}

bool StructPlayer::unSummonPet( struct StructPet * pPet )
{
	if( !pPet || !pPet->IsInWorld() )
		return false;

	if( pPet->IsMoving() )
	{
		// 안하면 이동 중에 역소환 됐을 때, 펫이 월드에 없는데도 IsMoving() 함수가 true를 반환함
		ArPosition pos = pPet->GetCurrentPosition( GetArTime() );
		pPet->SetMove( pos, 0 );
		pPet->StopMove();
	}

	TS_SC_UNSUMMON_PET msg;
	msg.handle = pPet->GetHandle();

	ArcadiaServer::Instance().Broadcast( pPet->GetRX(), pPet->GetRY(), pPet->GetLayer(), &msg );

	if( !IsVisibleRegion( pPet->GetRX(), pPet->GetRY(), GetRX(), GetRY() ) )
	{
		PendMessage( this, &msg );
	}

	pPet->SetSummonFlag( false );

	// 역소환
	RemovePetFromWorld( pPet );

	return true;
}

struct StructPet* StructPlayer::GetPet( int nPet_sid )
{
	std::vector< struct StructPet* >::iterator	it;

	for( it = m_vPetList.begin(); it != m_vPetList.end(); ++it )
	{
		if( (*it)->GetSID() == nPet_sid ) return *it;
	}

	return NULL;
}

void StructPlayer::procPendingUnSummonPet()
{
	// m_vPendingUnSummonPetList은 별도의 락을 두고 보호할 수 없으므로(지역 락을 플레이어 기준으로 걸 수 없으므로)
	// 아래와 같이 begin()만 erase하는 식으로 벡터를 순회해야 하며, m_vPendingUnSummonPetList에 대해 아래와 같은 사항을 보장해야 함
	// * m_vPendingUnSummonPetList에 대해 다른 함수/코드/쓰레드에서 clear, erase, insert 를 호출하면 안 됨(push_back만 가능)
	//   procPendingUnSummonPet 함수를 서로 다른 쓰레드에서 동시에 호출해서도 안된다는 것이 포함 됨

	// procPendingUnSummonPet 함수가 서로 다른 쓰레드에서 동시에 호출되는 것을 방지
	// 사정이 이렇다 보니 지역락 건 채로 procPendingUnSummonPet 함수 호출하면 데드락 걸림(csPendingUnSummonPet -> 지역 락)
	static XCriticalSection csPendingUnSummonPet;
	THREAD_SYNCHRONIZE( csPendingUnSummonPet );

	while( m_vPendingUnSummonPetList.begin() != m_vPendingUnSummonPetList.end() )
	{
		StructPet * pPet = m_vPendingUnSummonPetList.front();

		if( !pPet->IsInWorld() )
		{
			m_vPendingUnSummonPetList.erase( m_vPendingUnSummonPetList.begin() );
			continue;
		}

		ARCADIA_LOCK( ArcadiaServer::Instance().LockObjects( this, pPet ) );

		// 락 걸고 다시 검사
		if( !pPet->IsInWorld() )
		{
			m_vPendingUnSummonPetList.erase( m_vPendingUnSummonPetList.begin() );
			continue;
		}

		unSummonPet( pPet );
		m_vPendingUnSummonPetList.erase( m_vPendingUnSummonPetList.begin() );
	}
}

const CreatureStat &	StructPlayer::GetBaseStat() const
{
	int nStatID = 0;
	const JobInfo *pInfo = GameContent::GetJobInfo( GetJobId() );
	if( pInfo ) nStatID = pInfo->stat_id;

	return GameContent::GetStatInfo( nStatID );
}

struct _DEAD_PENALTY_POINT
{
	_DEAD_PENALTY_POINT( float _fEP, float _fDP ) : fEP( _fEP ), fDP( _fDP ) {}
	float fEP;
	float fDP;
};

static _DEAD_PENALTY_POINT getDeadPenaltyPoint( bool bIsPlayerPkOn, float fPlayerIP, int nLevelDiff )
{
	// 자세한 사항은 PK 기획 문서 참조
	float _PENALTY_POINT_TABLE[2][2][6] = 
	{
		/* 가해자									*/	{	//					피해자 PK off													
															//노멀,					블러디				데모니악					
															//EP		DP			EP		DP			EP		DP				
		/*							레벨차 10 미만	*/		{ 0.5f,		0.03f,		1.0f,	0.25f,		2.0f,	0.50f	},		
		/*							레벨차 10 이상	*/		{ 0.25f,	0.0f,		1.0f,	0.25f,		2.0f,	0.50f	},		
														} ,

														{						// 피해자 PK on
															//노멀,					블러디				데모니악					
															//EP		DP			EP		DP			EP		DP				
		/* pk Off					레벨차 10 미만	*/		{ 0.5f,		0.125f,		2.0f,	0.5f,		4.0f,	1.0f	},		
		/*							레벨차 10 이상	*/		{ 0.25f,	0.0f,		2.0f,	0.5f,		4.0f,	1.0f	},		
														}
	};

	int x, y, z;

	x = bIsPlayerPkOn ? 1 : 0;

	y = nLevelDiff < GameRule::nPKPenaltyLevel ? 0 : 1;

	z = 0;

	z += fPlayerIP >= GameRule::MORAL_LIMIT ? 2 : 0;
	z += fPlayerIP >= GameRule::CRIME_LIMIT ? 2 : 0;

	return _DEAD_PENALTY_POINT( _PENALTY_POINT_TABLE[x][y][z], _PENALTY_POINT_TABLE[x][y][z+1] );
}

void StructPlayer::onDead( StructCreature *pFrom, bool decreaseEXPOnDead )
{
	if( IsRiding() || HasRidingState() )
	{
		UnMount( UNMOUNT_FALL, pFrom );
	}

	if( IsBoothOpen() )
	{
		CloseBooth();
	}

	if( IsSitDown() )
	{
		StandUp();
	}

	AR_HANDLE taming_target = GetTamingTarget();

	if( taming_target )
	{
		StructMonster::iterator itMonster = StructMonster::get( taming_target );
		StructMonster * pMonster = ( (*itMonster) && (*itMonster)->IsMonster() ) ? static_cast< StructMonster * >(*itMonster) : NULL ;

		if( pMonster )
		{
			ClearTamer( pMonster );
		}
		else
		{
			SetTamingTarget( 0 );
		}
	}

	pFrom->OnKill( this );

	StructCreature::onDead( pFrom, decreaseEXPOnDead );

	StructPlayer * pKiller = NULL;

	// { 죽인 사람 찾기
	if( pFrom->IsPlayer() )
	{
		pKiller = static_cast< StructPlayer * >( pFrom );
	}
	else if( pFrom->IsSummon() )
	{
		pKiller = static_cast< StructSummon * >( pFrom )->GetMaster();
	}
	// }

	char * szKillerType = "UNKN";
	int nKillerID = 0;

	if( pFrom->IsPlayer() )		{ szKillerType = "PLYR"; nKillerID = static_cast< StructPlayer* >( pFrom )->GetPlayerUID(); }
	if( pFrom->IsMonster() )	{ szKillerType = "MNST"; nKillerID = static_cast< StructMonster* >( pFrom )->GetMonsterId(); }
	if( pFrom->IsSummon() )		{ szKillerType = "SUMN"; nKillerID = static_cast< StructSummon* >( pFrom )->GetSummonSID(); }	

	__int64 prev_exp = this->GetEXP();

	StructItem * pDropWearItem = NULL;
	StructItem * pDropInvenItem = NULL;

	int nKillerGetChaos = 0;

	if( pKiller ) nKillerGetChaos = pKiller->GetChaos();
	int nChaos = GetChaos();

	if( !IsInBattleArena() && !IsInDeathmatch() && ( ( !IsInBattleField() && !IsInStartedCompete( true ) ) || ( !pKiller ) ) )
	{
		// { 경험치 감소/아이템 드랍 처리
		procDecreaseEXPAndDropItem( pKiller, &pDropWearItem, &pDropInvenItem );
		// }

		if( pKiller ) nKillerGetChaos = pKiller->GetChaos() - nKillerGetChaos;
		nChaos = nChaos - GetChaos();

		if( pKiller != this )
			ProcImmoralPoint( pKiller, nKillerGetChaos );

		// 장착 중인 아이템의 에테리얼 내구도 감소 처리(자살이거나 몬스터에게 죽은 경우, 던전 시즈/레이드는 예외)
		if( ( !pKiller || pKiller == this ) && !IsInSiegeOrRaidDungeon() )
		{
			bool bNeedToCalculateStat = false;

			for( int i = 0 ; i < ItemBase::WEAR_FACE ; ++i )
			{
				StructItem * pItem = GetWearedItem( static_cast< ItemBase::ItemWearType >( i ) );

				if( !pItem || !pItem->GetMaxEtherealDurability() || !pItem->GetCurrentEtherealDurability() )
					continue;

				// 최대 내구도의 10% 감소(곱하기 상수화하면 소수자리 오차 발생하므로 나누기 정수 연산으로 적용함)
				int nDecrement = pItem->GetMaxEtherealDurability() / 10;

				pItem->AddCurrentEtherealDurability( nDecrement * -1 );
				SendItemMessage( this, pItem );

				bNeedToCalculateStat |= ( pItem->GetCurrentEtherealDurability() == 0 );
			}

			for( int i = 0; i < m_nBeltSlotMax; ++i )
			{
				StructItem *pItem = m_aBeltSlotCard[i];

				if( !pItem || !pItem->IsEquipmentOnBelt() )
					continue;

				// 최대 내구도의 10% 감소(곱하기 상수화하면 소수자리 오차 발생하므로 나누기 정수 연산으로 적용함)
				int nDecrement = pItem->GetMaxEtherealDurability() / 10;

				pItem->AddCurrentEtherealDurability( nDecrement * -1 );
				SendItemMessage( this, pItem );

				bNeedToCalculateStat |= ( pItem->GetCurrentEtherealDurability() == 0 );
			}

			if( bNeedToCalculateStat )
				CalculateStat();
		}

		if( pDropWearItem )
		{
			// 드랍 아이템 시스템 메시지 방송
			if( pKiller )
			{
				PrintfChatMessage( false, CHAT_NORMAL, "@BATTLE", this, "PK_KILLED_ITEM_DROP|%s|%d", pKiller->GetName(), pDropWearItem->GetItemCode() );
			}
		}

		if( pDropInvenItem )
		{
			// 드랍 아이템 시스템 메시지 방송
			if( pKiller )
			{
				PrintfChatMessage( false, CHAT_NORMAL, "@BATTLE", this, "PK_KILLED_ITEM_DROP|%s|%d", pKiller->GetName(), pDropInvenItem->GetItemCode() );
			}
		}
	}
	else if( GetCompeteID() )
	{
		CompeteManager::Instance().RetireCompeteWithPlayer( this, COMPETE_END_BY_DEATH );
		SetCompeteDead( true );
		SetLastDecreasedEXP( 0 );	// 안하면 부활 스킬쓰면 경험치 증가함
	}
	else if( IsInDeathmatch() && pKiller )
	{
		SetLastDecreasedEXP( 0 );
		std::string strFlagBuffer;

		int dead = atoi( GetFlag( "dead" ).c_str() ) + 1;
		XStringUtil::Format( strFlagBuffer, "%d", dead );
		SetFlag( "dead", strFlagBuffer.c_str() );

		if( pKiller && pKiller != this )
		{
			int kill = atoi( pKiller->GetFlag( "kill" ).c_str() ) + 1;
			XStringUtil::Format( strFlagBuffer, "%d", kill );
			pKiller->SetFlag( "kill", strFlagBuffer.c_str() );

			int nHealHP = pKiller->GetMaxHP() * GameRule::HEALING_RATE_ON_KILL_IN_DEATHMATCH;
			int nHealMP = pKiller->GetMaxMP() * GameRule::HEALING_RATE_ON_KILL_IN_DEATHMATCH;

			pKiller->AddHP( nHealHP );
			pKiller->AddMP( nHealMP );

			BroadcastHPMPMsg( pKiller, nHealHP, nHealMP, true );
			PrintfLocalChatMessage( false, CHAT_NOTICE, this->GetRX(), this->GetRY(), this->GetLayer(), "@SYSTEM", "@9244\v#@winner_name@#\v%s\v#@loser_name@#\v%s", pKiller->GetName(), this->GetName() );
		}
	}
	else if( IsInBattleArena() && pKiller )
	{
		SetLastDecreasedEXP( 0 );
		BattleArenaManager::Instance().OnPlayerDead( this, pKiller );
	}

	LOG::Log11N4S( LM_CHARACTER_DEATH, GetAccountID(), GetSID(), 0, nKillerID, GetLastDecreasedEXP(), nChaos, GetChaos(), 0, GetX(), GetY(), GetEXP(), GetAccountName(), LOG::STR_NTS, GetName(), LOG::STR_NTS, pKiller ? pKiller->GetName() : "", LOG::STR_NTS, szKillerType, LOG::STR_NTS );

	char buf[21];
	s_sprintf( buf, _countof( buf ), "%I64d", GetLastDecreasedEXP() );
	// { 사망 핸들러 호출
	std::string		strDeadHandler;
	strDeadHandler	= "on_player_dead( '";
	strDeadHandler	+= GetName();
	strDeadHandler	+= "', ";
	strDeadHandler	+= buf;
	strDeadHandler	+= " )";

	ThreadPlayerHelper TPHelper( this );
	LUA()->RunString( strDeadHandler.c_str() );
	// }

	if( GetPartyID() != 0 ) 
	{
		int nPartyID = GetPartyID();

		m_nPrevPartyBroadcastedHP = GetHP();
		m_nPrevPartyBroadcastedMP = GetMP();
		BroadcastPartyMemberInfo( nPartyID, this );

		if( PartyManager::GetInstance().IsLinkedParty( nPartyID ) )
			BroadcastLinkedPartyMemberInfo( nPartyID, this );
	}

	// 사망시 소환된 소환수 바로 역소환하도록 대기
	// Lock 문제로 인해 플래그 세팅 후 onProcess에서 처리하도록 함
	bool bUnSummonPended = false;
	if( GetSubSummon() && GetSubSummon()->IsInWorld() )
	{
		PendUnSummon( GetSubSummon() );
		bUnSummonPended = true;
	}
	if( GetMainSummon() && GetMainSummon()->IsInWorld() )
	{
		PendUnSummon( GetMainSummon() );
		bUnSummonPended = true;
	}

	if( bUnSummonPended )
		ArcadiaServer::Instance().SetObjectPriority( this, ArSchedulerObject::UPDATE_PRIORITY_HIGHEST );
}

void StructPlayer::onHPChange( int nPrevHP )
{
	if( GetPartyID() != 0 ) 
	{
		int nPartyID = GetPartyID();

		int nHPGap = GetHP() - m_nPrevPartyBroadcastedHP;
		int nTh = GetMaxHP()/10;
		if( nHPGap < 0 ) nHPGap = 0 - nHPGap;
		if( nHPGap > nTh )
		{
			m_nPrevPartyBroadcastedHP = GetHP();
			BroadcastPartyMemberInfo( nPartyID, this );
		}

		if( !nPrevHP && PartyManager::GetInstance().IsLinkedParty( nPartyID ) )
		{
			BroadcastLinkedPartyMemberInfo( nPartyID, this );
		}
	}

	StructCreature::onHPChange( nPrevHP );
}

void StructPlayer::onMPChange( int nPrevMP )
{
	if( GetPartyID() != 0 ) 
	{
		int nMPGap = GetMP() - m_nPrevPartyBroadcastedMP;
		int nTh = GetMaxMP()/10;
		if( nMPGap < 0 ) nMPGap = 0 - nMPGap;
		if( nMPGap > nTh )
		{
			m_nPrevPartyBroadcastedMP = GetMP();
			BroadcastPartyMemberInfo( GetPartyID(), this );
		}
	}

	StructCreature::onMPChange( nPrevMP );
}

const unsigned short StructPlayer::ChangeGold( const StructGold & gold )
{
	if( m_nGold == gold )
		return RESULT_SUCCESS;

	if( gold > GameRule::MAX_GOLD_FOR_INVENTORY )
		return RESULT_TOO_MUCH_MONEY;

	if( gold < StructGold( 0 ) )
		return RESULT_TOO_CHEAP;

	m_nGold = gold;

	SendGoldChaosUpdateMsg( this );

	m_TitleManager.UpdateTitleConditionByGold( m_nGold );

	return RESULT_SUCCESS;
}

const unsigned short StructPlayer::ChangeStorageGold( const StructGold & gold )
{
	if( m_nStorageGold == gold )
		return RESULT_SUCCESS;

	if( gold > GameRule::MAX_GOLD_FOR_STORAGE )
		return RESULT_TOO_MUCH_MONEY;

	if( gold < StructGold( 0 ) )
		return RESULT_TOO_CHEAP;

	m_nStorageGold = gold;
	DBQuery( new DB_UpdateStorageGold( this ) );

	SendPropertyMessage( this, GetHandle(), "storage_gold", m_nStorageGold.GetRawData() );
	return RESULT_SUCCESS;
}

void StructPlayer::onJobLevelUp()
{
	SendPropertyMessage( this, GetHandle(), "job_level", GetJobLevel() );

	m_QuestManager.UpdateQuestStatusByJobLevel( GetJobDepth(), GetJobLevel() );
}

void StructPlayer::onJPChange()
{
	SendPropertyMessage( this, GetHandle(), "jp", GetJobPoint() );
}

void StructPlayer::onResetSkill( const int jobDepth )
{
	__int64 returnedJobLevelJP = GetAllJobLevelJP();
	SetJP( GetJobPoint() + returnedJobLevelJP );

	SetJobLevel( 1 );

	for( int i = GetCurrentPassiveSkillLevel( StructSkill::SKILL_CREATURE_CONTROL ); i < 6; ++i )
	{
		StructItem *pSummonCard = GetSummonCardAt( i );
		if( pSummonCard && pSummonCard->GetSummonStruct() )
		{
			StructSummon *pSummon = pSummonCard->GetSummonStruct();

			// 소환수 역소환(PendUnSummon으로 지역 락 관련 문제 발생 방지 - 즉시 역소환 됨을 보장하지는 않음)
			if( pSummon->GetSummonFlag() )
			{
				PendUnSummon( pSummon );
				ArcadiaServer::Instance().SetObjectPriority( this, ArSchedulerObject::UPDATE_PRIORITY_HIGHEST );
			}

			// 소환수가 입고 있던 장비 해제
			for( int x = 0 ; x < ItemBase::MAX_ITEM_WEAR ; ++x )
			{
				StructItem *pItem = pSummon->GetWearedItem( (ItemBase::ItemWearType)x );
				if( pItem ) 
				{
					pSummon->Putoff( (ItemBase::ItemWearType)x );
				}
			}

			SetSummonCardAt( i, NULL );
		}
	}

	// 스킬이 초기화되면 Bind되어 있던 스킬카드가 UnBind
	// 스킬 카드 시스템의 주체가 아이템이므로 아이템 기준
	for( size_t i = 0; i < m_Inventory.GetCount(); ++i )
	{
		StructItem *pItem = m_Inventory.Get( i );
		if( pItem->IsSkillCard() )
		{
			if( pItem->GetBindedPlayerSID() == GetPlayerUID() && GetCurrentPassiveSkillLevel( pItem->GetSkillId() ) == 0 )
			{
				UnBindSkillCard( pItem );
			}
		}
	}

	SendCreatureEquipMessage( this, false );

	Save( true );

	::SendSkillMessage( this, this, -1, true );
}

void StructPlayer::onRemoveSkill( StructSkill* removedSkill )
{
	DBQuery( new DB_DeleteSkill( this, removedSkill->GetSkillUID() ) );
}

bool StructPlayer::IsChangeableJob( const int nTargetJob ) const
{
	const JobInfo *pTargetInfo = GameContent::GetJobInfo( nTargetJob );

	// 직업 정보가 존재하지 않음
	if( !pTargetInfo ) return false;

	const int nJobDepth = GetJobDepth();
	const int nTargetJobDepth = pTargetInfo->job_depth;
	const int nCurrentJobID = ( nJobDepth == nTargetJobDepth ) ? GetJobId() : m_nPrevJobId[ nTargetJobDepth ];

	// 동일한 직업으로의 전직이거나 차수를 건너뛰는 전직은 불가
	if( nTargetJob == nCurrentJobID ) return false;
	if( nTargetJobDepth > nJobDepth + 1 ) return false;
	if( nTargetJobDepth >= GameRule::MAX_JOB_DEPTH ) return false;

	if( nTargetJobDepth > 0 )
	{
		int nPrevJob = ( nTargetJobDepth - 1 == nJobDepth ) ? GetJobId() : m_nPrevJobId[ nTargetJobDepth - 1 ];
		int nPrevJobLevel = ( nTargetJobDepth - 1 == nJobDepth ) ? GetJobLevel() : m_nPrevJobLevel[ nTargetJobDepth - 1 ];
		const JobInfo *pPrevInfo = GameContent::GetJobInfo( nPrevJob );

		// 대상의 하위 직업이 요구하는 대상 직업 조건
		// - 직업 트리 조건
		bool bChangeable = false;
		for( int i = 0; i < 4; ++i )
		{
			if( pPrevInfo->availble_job[i] == nTargetJob ) { bChangeable = true; break; }
		}
		if( !bChangeable ) return false;

		// - 레벨 조건
		if( GetLevel() < pPrevInfo->up_lv || nPrevJobLevel < pPrevInfo->up_jlv ) return false;
	}

	if( nTargetJobDepth < nJobDepth )
	{
		int nNextJob = ( nTargetJobDepth + 1 == nJobDepth ) ? GetJobId() : m_nPrevJobId[ nTargetJobDepth + 1 ];
		int nNextJobLevel = m_nPrevJobLevel[ nTargetJobDepth ];

		// 대상 직업이 요구하는 대상의 상위 직업 조건
		// - 직업 트리 제한
		bool bChangeable = false;
		for( int i = 0; i < 4; ++i )
		{
			if( pTargetInfo->availble_job[i] == nNextJob ) { bChangeable = true; break; }
		}
		if( !bChangeable ) return false;

		// 대상 직업이 바뀌더라도 상위 직업으로 전직할 수 있는지에 대한 조건
		// 만약 대상 직업이 바뀜으로 인해 상위 직업으로의 전직이 성립하지 않는다면 대상 직업을 바꿔서는 안 된다.
		// - 레벨 조건
		if( GetLevel() < pTargetInfo->up_lv || nNextJobLevel < pTargetInfo->up_jlv ) return false;
	}

	return true;
}

bool StructPlayer::ChangeJob( const _SKILL_RESET_METHOD eMethod, const int nTargetJob )
{
	// 전직 불가 직업
	if( !IsChangeableJob( nTargetJob ) )
		return false;

	const JobInfo *pInfo = GameContent::GetJobInfo( nTargetJob );

	// 스킬 트리가 변경될 것이므로 해당 차수까지의 스킬 초기화
	int nPrevJobID;
	int nPrevJobLevel = GetJobLevel();
	int nPrevTP = GetTalentPoint();
	int nTargetJobDepth = pInfo->job_depth;
	ResetSkill( eMethod, nTargetJobDepth );

	// 현재 직업이 바뀌었다면 직업 변경에 대한 방송도 필요하다. 또한 기본 스탯이 변경되므로 CalculateStat도 호출해야 한다.
	if( GetJobDepth() <= nTargetJobDepth )
	{
		onBeforeResetJob();

		nPrevJobID = m_nJob;

		if( GetJobDepth() < nTargetJobDepth )
		{
			m_nPrevJobId[ nTargetJobDepth - 1 ] = nPrevJobID;
			m_nPrevJobLevel[ nTargetJobDepth - 1 ] = nPrevJobLevel;
			m_nJobLevel = 1;
			m_nJobDepth = nTargetJobDepth;
		}

		// 직업 정보 리셋
		m_nJob = nTargetJob;

		onAfterResetJob();

		CalculateStat();
	}
	// 이전 직업이 바뀌었다면 직업 변경에 대한 방송은 필요하지 않으며 변경된 이전 직업에 대한 정보만 변경한다.
	else
	{
		nPrevJobID = m_nPrevJobId[ nTargetJobDepth ];
		m_nPrevJobId[ nTargetJobDepth ] = nTargetJob;

		char buf[30];
		s_sprintf( buf, _countof( buf ), "job_%d", nTargetJobDepth );
		SendPropertyMessage( this, GetHandle(), buf, nTargetJob );

		Save();
	}

	LOG::Log11N4S( LM_CHARACTER_CHANGE_JOB, GetAccountID(), GetSID(), 0, nPrevJobID, nTargetJob, nPrevJobLevel, GetJobLevel(), nPrevTP, GetTalentPoint(), 0, 0, GetAccountName(), LOG::STR_NTS, GetName(), LOG::STR_NTS, "", 0, "", 0 );

	return true;
}

// 임시로 StructCreature에 있는 함수 전방 선언으로 끌고 옴
// TODO: 어디 넣을만한 적당한 위치 없나?
const int GetAllowedMaxSkillLevel( const StructCreature* creature, const StructSkill* skill, const int remainingMaxJobDepth );

bool StructPlayer::ResetJob( const int nTargetJobDepth )
{
	if( GetJobDepth() <= nTargetJobDepth )
	{
		return false;
	}

	if( IsUsingSkill() )
	{
		CancelSkill();
	}

	struct NotOwnedByRemainingSkillTree
	{
		NotOwnedByRemainingSkillTree( const StructCreature* creature, const int remainingMaxJobDepth )
			: creature( creature ), remainingMaxJobDepth( remainingMaxJobDepth ) {}

		const bool operator() ( const StructSkill* skill ) const
		{
			// 남아 있어야 하는 스킬 트리에서 허용 레벨보다 실제 레벨이 더 높다 -> 초기화 불가능
			return ( skill->GetSkillUID() >= 0 ) && 
				GetAllowedMaxSkillLevel( creature, skill, remainingMaxJobDepth ) < skill->GetBaseSkillLevel(); 
		}
		const StructCreature* creature;
		int remainingMaxJobDepth;
	};

	std::vector< StructSkill* >::iterator it =
		std::find_if( m_vAllSkillList.begin(), m_vAllSkillList.end(), NotOwnedByRemainingSkillTree( this, nTargetJobDepth - 1 ) );

	if( it != m_vAllSkillList.end() )
	{
		return false;
	}

	for( std::vector< std::pair<StructSkill *, int> >::iterator itAura = m_vAura.begin(); itAura != m_vAura.end();  ++itAura )
	{
		SendAuraMsessage( this, itAura->first->GetSkillId() );
	}

	m_vAura.clear();

	__int64 nJP = getJPAfterJobReset( nTargetJobDepth );
	int nTP = getTPAfterJobReset( GetJobDepth() );

	onBeforeResetJob();

	// 타겟 직업 정보 보관
	int nTargetJob = m_nPrevJobId[ nTargetJobDepth ];

	// 전직 이전의 각 직업 정보 및 JLv 정보 초기화
	for( int nJobDepth = nTargetJobDepth ; nJobDepth < GetJobDepth() ; ++nJobDepth )
	{
		m_nPrevJobId[ nJobDepth ] = 0;
		m_nPrevJobLevel[ nJobDepth ] = 0;
	}

	// 직업 정보 리셋
	SetJP( nJP );
	if( nTP != GetTalentPoint() )
	{
		LOG::Log11N4S( LM_CHARACTER_USE_TP, GetAccountID(), 
			GetSID(), GetJobId(), 
			GetJobLevel(), nTargetJob,
			1, 0,
			GetTalentPoint(), nTP, 
			0, 0,
			GetAccountName(), LOG::STR_NTS, 
			GetName(), LOG::STR_NTS,
			"", 0,
			"ByDowngradingFromMasterClass", LOG::STR_NTS );
		SetTalentPoint( nTP );
	}
	m_nJob = nTargetJob;
	m_nJobDepth = nTargetJobDepth;
	SetJobLevel( 1 );

	onAfterResetJob();

	CalculateStat();

	LOG::Log11N4S( LM_CHARACTER_CHANGE_JOB, GetAccountID(), GetSID(), 0, GetPrevJobId( m_nJobDepth - 1 ), GetJobId(), GetPrevJobLevel( m_nJobDepth - 1 ), GetJobLevel(), nTP, GetTalentPoint(), 0, 0, GetAccountName(), LOG::STR_NTS, GetName(), LOG::STR_NTS, "", 0, "", 0 );

	return true;
}

bool StructPlayer::SetRace( const _SKILL_RESET_METHOD eMethod, const unsigned char nTargetRace )
{
	if( GetRace() == nTargetRace )
	{
		return false;
	}

	int nTargetJob;
	switch( nTargetRace )
	{
	case JobInfo::GAIA	:	nTargetJob = JobInfo::GAIA_BASIC_JOB; break;
	case JobInfo::DEVA	:	nTargetJob = JobInfo::DEVA_BASIC_JOB; break;
	case JobInfo::ASURA	:	nTargetJob = JobInfo::ASURA_BASIC_JOB; break;
	default				:	assert( 0 ); return false;
	}

	// 종족 변환 전 로그용 데이터
	int nPrevRace = GetRace();
	int nPrevJob = GetJobId();
	int nPrevJobLevel = GetJobLevel();
	int nPrevTP = GetTalentPoint();

	// 우선 0차 직업으로 돌아간다.
	ResetSkill( eMethod, 0 );
	ResetJob( 0 );

	// 현재 종족의 0차 직업에서 대상 종족의 0차 직업으로 직업 변환을 한다.
	ChangeJob( eMethod, nTargetJob );

	onBeforeResetRace();

	// 종족 정보 리셋
	m_nRace = nTargetRace;

	onAfterResetRace();

	LOG::Log11N4S( LM_CHARACTER_CHANGE_RACE, GetAccountID(), GetSID(), nPrevRace, nTargetRace, nPrevJob, nTargetJob, nPrevJobLevel, GetJobLevel(), nPrevTP, GetTalentPoint(), 0, GetAccountName(), LOG::STR_NTS, GetName(), LOG::STR_NTS, "", 0, "", 0 );

	return true;
}

__int64 StructPlayer::getJPAfterJobReset( int nTargetJobDepth )
{
	if( GetJobDepth() <= nTargetJobDepth )
	{
		return 0;
	}

	// 현재 JP와 현재 직업에서의 JLv 달성에 필요한 JP 계산
	__int64 nJP = GetJobPoint() + GetAllJobLevelJP();

	// 전직 이전의 각 직업에서의 JLv 달성에 필요한 JP 계산
	for( int nJobDepth = nTargetJobDepth ; nJobDepth < GetJobDepth() ; ++nJobDepth )
	{
		int nPrevJobLevel = m_nPrevJobLevel[ nJobDepth ];

		for( int i = 1 ; i < nPrevJobLevel ; ++i )
		{
			nJP += GameContent::GetNeedJpForJobLevelUp( i, nJobDepth );
		}
	}

	return nJP;
}

void StructPlayer::onAfterResetJob()
{
	m_QuestManager.UpdateQuestStatusByJobLevel( GetJobDepth(), GetJobLevel() );

	// 마스터 클래스가 되는 순간에 특성 포인트를 지급한다.
	// 만약 마스터 클래스보다 더 높은 차수의 직업이 생긴다면 직업을 초기화하면서 문제가 생기겠지만 당분간 생길 일이 없으며,
	// 또한 직업 초기화는 스크립트를 통하지 않고 서버에 처리하므로 추가적으로 포인트를 얻지 못 할 것이다.
	int nTP = m_nJobDepth == GameRule::MIN_TALENT_POINT_JOB_DEPTH ? GameRule::DEFAULT_TALENT_POINT : 0;
	if( nTP ) 
	{
		LOG::Log11N4S( LM_CHARACTER_GAIN_TP, GetAccountID(), 
			GetSID(), 0, 
			GetPrevJobId( GetJobDepth() - 1 ), GetJobId(),
			GetPrevJobLevel( GetJobDepth() - 1 ), GetJobLevel(),
			GetTalentPoint(), GetTalentPoint() + nTP, 
			0, 0,
			GetAccountName(), LOG::STR_NTS, 
			GetName(), LOG::STR_NTS,
			"", 0,
			"ByMasterClassJobChange", LOG::STR_NTS );
		SetTalentPoint( GetTalentPoint() + nTP );
	}

	// 길드 정보 갱신
	if( IsInGuild() )
		// 직업만 바뀌었으니 직업 변경 메시지만 보내자(길드 정보에 보관된 정보도 갱신할 겸)
		//BroadcastGuildMemberInfo( GetGuildID(), this );
		GuildManager::GetInstance().OnChangeCharacterJob( GetGuildID(), GetPlayerUID(), GetJobId() );

	// 파티 정보 갱신
	if( IsInParty() )
		// 직업만 바뀌었으니 직업 변경 메시지만 보내자(파티 정보에 보관된 정보도 갱신할 겸)
		//BroadcastPartyMemberInfo( GetPartyID(), this );
		PartyManager::GetInstance().OnChangeCharacterJob( GetPartyID(), GetPlayerUID(), GetJobId() );

	Save();

	BroadcastPropertyMessage( GetRX(), GetRY(), GetLayer(), GetHandle(), "job", GetJobId() );
	SendJobInfo( this, this );
}

void StructPlayer::onAfterResetRace()
{
	DBQuery( new DB_UpdateRace( this ) );

	BroadcastPropertyMessage( GetRX(), GetRY(), GetLayer(), GetHandle(), "race", GetRace() );
}

void StructPlayer::onRegisterSkill( int skill_uid, int skill_id, int skill_level, bool is_new_skill )
{
	SkillBase *pBase = GameContent::GetSkillBase( skill_id );
	if( pBase && pBase->IsSystemSkill() ) return;

	if( is_new_skill )
	{
		// 새로 배우는거면 insert
		DBQuery( new DB_InsertSkill( this, skill_uid, GetPlayerUID(), 0/*JP*/, 0, skill_id, skill_level, 0 ) );
	}
	else
	{
		// 아니면 update
		DBQuery( new DB_UpdateSkill( this, skill_uid, skill_level, GetRemainCoolTime( skill_id ) ) );
	}

	int skill_type = pBase->GetSkillEffectType();

	if(	skill_id == StructSkill::SKILL_CREATURE_MASTERY )
	{
		setSummonUpdate();
	}

	// TODO 스킬 배우는 퀘스트 처리
	m_QuestManager.UpdateQuestStatusBySkillLevel( skill_id, skill_level );

	m_TitleManager.UpdateTitleConditionBySkillLevel( skill_id, skill_level );

	::SendSkillMessage( this, this, skill_id );
}

int StructPlayer::onDamage( StructCreature * pFrom, Elemental::Type elementalType, DamageType damageType, int nDamage, bool bCritical, bool bDouble)
{
	// 기본 물리 타격/마법 공격 데미지에 의해서만 앉기 해제/라이딩 상태에서 떨어지도록
	if( nDamage > 0 &&
		( damageType == DT_NORMAL_PHYSICAL_DAMAGE || damageType == DT_NORMAL_MAGICAL_DAMAGE || damageType == DT_NORMAL_PHYSICAL_LEFT_HAND_DAMAGE || damageType == DT_NORMAL_PHYSICAL_SKILL_DAMAGE ) )
	{
		if( IsRiding() || HasRidingState() )
		{
			if( ( bCritical && XRandom() % 100 <= GetUnMountProbabilityOnCriticalDamage() ) || XRandom() % 100 <= GetUnMountProbabilityOnDamage() )
				UnMount( UNMOUNT_FALL, pFrom );
		}
		else if( IsSitDown() )
		{
			StandUp();
			BroadcastStatusMessage( this );

			AR_TIME t = GetArTime();
			AddState( StructState::CARELESSNESS, 0, 1, t, t + 300 );
		}
	}

	StructSummon* pMainSummon = GetMainSummon();
	if( m_fPassDamageRatio && pMainSummon != NULL )
	{
		int nPassDamage = std::min( int( nDamage * m_fPassDamageRatio ), pMainSummon->GetHP() );
		if( nPassDamage )
		{
			// 시야내에 있다면
			if( IsVisibleRegion( GetRX(), GetRY(), pMainSummon->GetRX(), pMainSummon->GetRY() ) != 0 )
			{
				pMainSummon->damage( pFrom, nPassDamage );
				nDamage -= nPassDamage;

				BroadcastHPMPMsg( pMainSummon, 0 - nPassDamage, 0 );
			}
		}
	}

	AR_TIME t = GetArTime();
	ArPosition pos = GetCurrentPosition( t );

	if( IsInStartedCompete( true ) )
	{
		StructPlayer * pAttackPlayer = NULL;
		if( pFrom->IsPlayer() )
			pAttackPlayer = static_cast< StructPlayer * >( pFrom );
		else if( pFrom->IsSummon() )
			pAttackPlayer = static_cast< StructSummon * >( pFrom )->GetMaster();

		if( !pAttackPlayer || GetCompeteID() != pAttackPlayer->GetCompeteID() )
		{
			CompeteManager::Instance().RetireCompeteWithPlayer( this, COMPETE_END_BY_INTERRUPT );
		}
	}

	return StructCreature::onDamage( pFrom, elementalType, damageType, nDamage, bCritical, bDouble );
}

void StructPlayer::SetDialogTitle( const char *szString, int type )
{
	if( !szString ) return;

	m_nDialogType = type;

	if( m_nDialogType ) m_bNonNPCDialog = true;

	m_strDialogTitle = szString;
	m_strDialogMenu.clear();
	m_strSpecialDialogMenu.clear();
}

void StructPlayer::SetFixedDialogTrigger( const char *szString )
{
	m_strFixedDialogTrigger = szString;
}

bool StructPlayer::IsFixedDialogTrigger( const char * szTrigger )
{
	if( m_strFixedDialogTrigger.size() && strstr( szTrigger, m_strFixedDialogTrigger.c_str() ) )
		return true;

	return false;
}

void StructPlayer::ClearFixedDialogTrigger()
{
	m_strFixedDialogTrigger.clear();
}

void StructPlayer::SetDialogText( const char *szString )
{
	if( !szString ) return;

	m_strDialogText = szString;
	m_strDialogMenu.clear();
	m_strSpecialDialogMenu.clear();
}

void StructPlayer::ClearDialogMenu()
{
	m_strDialogMenu = "";
}

void StructPlayer::SetSpecialDialogMenu( const char * szString )
{
	if( !szString ) return;

	m_strSpecialDialogMenu = szString;
}

bool StructPlayer::IsSpecialDialogMenu( const char * szString )
{
	return( m_strSpecialDialogMenu.size() && strstr( szString, m_strSpecialDialogMenu.c_str() ) );
}

void StructPlayer::ClearSpecialDialogMenu()
{
	m_strSpecialDialogMenu.clear();
}

void StructPlayer::AddDialogMenu( const char *szString, const char *szTrigger )
{
	if( !szString || !szTrigger ) return;

	if( strchr( szString, '\t' ) ) return;
	if( strchr( szTrigger, '\t' ) ) return;

	m_strDialogMenu += "\t";
	m_strDialogMenu += szString;
	m_strDialogMenu += "\t";
	m_strDialogMenu += szTrigger;
	m_strDialogMenu += "\t";
}

void StructPlayer::SetTrigger( const char *szTrigger )
{
	if( !szTrigger ) return;
	m_strDialogMenu = szTrigger;
}

const bool StructPlayer::SetupQuestDialog( const QuestBase::QuestCode nQuestCode, const int nTextID, const int nDialogType )
{
	// 랜덤 퀘스트일때 처리는 좀 달라야 하지만, 일단은 랜덤 퀘스트는 고려하지 않는다.ㅡ.ㅡ;;;

	// 어딘가 데이터가 잘못되어 있는 것이므로 해당 퀘스트 관련 데이터 중 누락된 부분을 확인하여 수정해야 함
	if( !nQuestCode || !nTextID )
	{
		assert( 0 );
		return false;
	}

	const QuestBase & rQuestBase = StructQuest::GetQuestBase( nQuestCode );

	if( rQuestBase.nCode != nQuestCode )
	{
		assert( 0 );
		return false;
	}

	char buf[128];
	int nProgress = QUEST_IS_STARTABLE;

	if( IsStartableQuest( nQuestCode ) )
	{
		time_t tRemainCoolTime = m_QuestManager.GetRemainQuestCoolTime( nQuestCode, time( NULL ) );

		if( tRemainCoolTime )
		{
			nProgress = QUEST_IS_IN_COOL_TIME;
		}

		nProgress = QUEST_IS_STARTABLE;
	}
	else if( IsFinishableQuest( nQuestCode ) )		nProgress = QUEST_IS_FINISHABLE;
	else											nProgress = QUEST_IS_IN_PROGRESS;

	// 최근 대화한 NPC가 해당 퀘스트 텍스트를 보여줄 수 있는 상태라면 타이틀에 NPC 이름 설정
	StructNPC* pNPC = static_cast< StructNPC* >( GameObject::raw_get( GetContactNPCHandle() ) );
	if( pNPC && pNPC->GetQuestTextId( nQuestCode, nProgress ) == nTextID )
	{
		SetDialogTitle( pNPC->GetName(), nDialogType );
	}
	// 관계된 NPC를 찾을 수 없다면 타이틀에 아무것도 나오지 않음
	else
	{
		SetDialogTitle( "", nDialogType );
	}

	// 내용 출력
	s_sprintf( buf, _countof( buf ), "QUEST|%d|%d", nQuestCode, nTextID );
	SetDialogText( buf );

	// 버튼
	// 임시 작업이므로 클라이언트 작업을 없애기 위해 쿨타임 중인 상태에서도 동일하게 방송을 해준다.
	std::string strButton, strTrigger;
	if( nProgress == QUEST_IS_STARTABLE || nProgress == QUEST_IS_IN_COOL_TIME )
	{
		XStringUtil::Format( strTrigger, "start_quest( %d, %d )", nQuestCode, nTextID );
		AddDialogMenu( "START", strTrigger.c_str() );
		AddDialogMenu( "REJECT", "" );
	}
	else if( nProgress == QUEST_IS_FINISHABLE )
	{
		int nSelectableRewardCount = 0;
		int i = 0;
		for( i = 0; i < QuestBase::MAX_OPTIONAL_REWARD; ++i )
		{
			if( !rQuestBase.OptionalReward[i].nItemCode ) break;
			nSelectableRewardCount++;
		}

		if( nSelectableRewardCount )	
		{
			strButton = "NULL";
			for( int i = 0; i < nSelectableRewardCount; ++i )
			{
				XStringUtil::Format( strTrigger, "end_quest( %d, %d )", nQuestCode, i );
				AddDialogMenu( strButton.c_str(), strTrigger.c_str() );
			}

			strButton = "REWARD";
			XStringUtil::Format( strTrigger, "%d", rQuestBase.nCode );
			AddDialogMenu( strButton.c_str(), strTrigger.c_str() );
		}
		else						
		{
			strButton = "NULL";
			XStringUtil::Format( strTrigger, "end_quest( %d, -1 )", nQuestCode );
			AddDialogMenu( strButton.c_str(), strTrigger.c_str() );

			strButton = "REWARD";
			XStringUtil::Format( strTrigger, "%d", rQuestBase.nCode );
			AddDialogMenu( strButton.c_str(), strTrigger.c_str() );
		}		
	}
	else
	{	
		AddDialogMenu( "OK", "" );
	}

	return true;
}

void StructPlayer::ShowDialog()
{
	if( m_strDialogTitle.empty() && m_strDialogText.empty() ) return;

	SendDialogMessage( this, GetContactNPCHandle(), m_nDialogType, m_strDialogTitle.c_str(), m_strDialogText.c_str(), m_strDialogMenu.c_str() );

	m_nDialogType = 0;
	m_strDialogTitle.clear();
	m_strDialogText.clear();	
}

bool StructPlayer::IsValidTrigger( const char *szTrigger )
{
	std::vector< std::string > strTriggerList;
	XStringUtil::Split( m_strDialogMenu.c_str(), strTriggerList, "\t", false );

	size_t idx;	
	for( idx = 0; idx < strTriggerList.size(); ++idx )
	{
		if( !(idx%2) ) continue;

		if( !strcmp( strTriggerList[idx].c_str(), szTrigger ) ) return true;
	}

	if( m_strSpecialDialogMenu.size() && strstr( szTrigger, m_strSpecialDialogMenu.c_str() ) )
		return true;

	return false;
}

void StructPlayer::PendWarp( int x, int y, unsigned char layer, int nInstanceDungeonType, int isForceWarp )
{
	m_nPendWarpX = x;
	m_nPendWarpY = y;
	m_nPendWarpLayer = layer;
	m_nPendWarpInstanceDungeonType = nInstanceDungeonType;
	m_bForceWarp = isForceWarp;
	SetInvincible( true );

	if( GetMainSummon() )
	{
		GetMainSummon()->SetInvincible( true );
	}

	if( GetSubSummon() )
	{
		GetSubSummon()->SetInvincible( true );
	}

	m_nWarpEndTime = GetArTime();
}

const unsigned short StructPlayer::PendWarpToHuntaholicLobby( const int nHuntaholicID )
{
	AR_TIME t = GetArTime();

	// 이미 헌터홀릭 로비 또는 던전 안에 있으면 입장 워프 불가
	if( HuntaholicManager::Instance().GetHuntaholicID( GetPos() ) )
		return RESULT_NOT_ACTABLE_IN_HUNTAHOLIC;

	// PK On 상태면 입장 불가
	if( IsPKOn() || IsPKOning() )
		return RESULT_PK_LIMIT;

	ArPosition posLobby( HuntaholicManager::Instance().GetLobbyPosition( nHuntaholicID ) );

	// 데이터에 없는 놈이라면 처리 불가
	if( !posLobby.GetX() || !posLobby.GetY() )
		return RESULT_NOT_EXIST;

	unsigned char nLayer = HuntaholicManager::Instance().GetProperLobbyLayer( nHuntaholicID, GetLevel() );

	if( nLayer == GameRule::HUNTAHOLIC_UNUSABLE_LOBBY_LAYER )
		return RESULT_NOT_ENOUGH_LEVEL;

	// 중독 방지 시간으로 인한 입장 제한 처리
	if( IsGameTimeLimited() && GetContinuousPlayTime() >= GameRule::nMaxTiredGameTime )
		return RESULT_GAMETIME_LIMITED;

	ArPosition posCurrent( GetCurrentPosition( t ) );

	// 복귀 지점 설정 및 HP/MP
	StoreCurrentStatesOnEnterInstanceGame( true );

	SetMove( posCurrent, 0 );
	PendWarp( posLobby.GetX(), posLobby.GetY(), nLayer );

	ArcadiaServer::Instance().SetObjectPriority( this, UPDATE_PRIORITY_HIGHEST );

	return RESULT_SUCCESS;
}

void StructPlayer::PendWarpToDeathmatch( int instance_game_type )
{
	// 입장할 데스매치 좌표 얻어오기
	ArPosition posDeathmatch = GameContent::GetDeathmatchPosition( GetLevel(), instance_game_type );

	unsigned char nLayer = GetLayer();

	int nCurrentChannel = ChannelManager::GetChannelId( GetX(), GetY() );
	int nTargetChannel = ChannelManager::GetChannelId( posDeathmatch.GetX(), posDeathmatch.GetY() );
	if( nTargetChannel && nCurrentChannel != nTargetChannel && ChannelManager::GetChannelType( nTargetChannel ) == ChannelManager::TYPE_USER_LIMIT )
	{
		nLayer = ChannelManager::GetProperLayer( posDeathmatch.GetX(), posDeathmatch.GetY() );
	}

	PendWarp( posDeathmatch.GetX(), posDeathmatch.GetY(), nLayer );

	ArcadiaServer::Instance().SetObjectPriority( this, UPDATE_PRIORITY_HIGHEST );
}

bool StructPlayer::IsPartyInvitable( StructPlayer * pTarget ) const
{
	// PK 서버면 Demoniac/Bloody 케릭을 노멀 케릭으로 간주한다.
	bool bIsDemoniacOrBloody = ( !GameRule::bIsPKServer && ( IsDemoniacCharacter() || IsBloodyCharacter() ) ) ? true : false;
	bool bIsTargetDemoniacOrBloody = ( !GameRule::bIsPKServer && ( pTarget->IsDemoniacCharacter() || pTarget->IsBloodyCharacter() ) ) ? true : false;

	// 이미 서로 파티. 이 체크는 PK 룰 체크에서 할 일이 아니므로 일단 그냥 true 리턴.
	if( m_nPartyID && m_nPartyID == pTarget->m_nPartyID )
	{
		return true;
	}
	// 길드원/길드 연합
	else if( GetGuildID() && ( GetGuildID() == pTarget->GetGuildID() ) || GuildManager::GetInstance().GetAllianceID( GetGuildID() ) == GuildManager::GetInstance().GetAllianceID( pTarget->GetGuildID() ) )
	{
		// PKON이 아닌 케릭터끼리이거나 서로 PKON만 가능 -> PKON 상태가 서로 같으면 가능
		if( m_bIsPK != pTarget->m_bIsPK )
			return false;
		else
			return true;
	}
	// 관계 없는 사이
	else
	{
		// PKON이 아닌 케릭터끼리만 가능
		if( bIsDemoniacOrBloody || bIsTargetDemoniacOrBloody || m_bIsPK || pTarget->m_bIsPK )
			return false;
		else
			return true;
	}
}

bool StructPlayer::IsGuildInvitable( StructPlayer * pTarget ) const
{
	if( m_bIsPK || pTarget->m_bIsPK )
		return false;
	else
		return true;
}

std::string StructPlayer::GetFlag( const char *szKey ) const
{
	THREAD_SYNCRONIZE( m_csHash );

	std::string strResult;
	m_hsValue.lookup( szKey, strResult );
	return strResult;
}

void StructPlayer::SetFlag( const char *szKey, const char *szData )
{
	THREAD_SYNCRONIZE( m_csHash );

	if( m_hsValue.has( szKey ) )
	{
		m_hsValue.modify( szKey, szData );
	}
	else
	{
		m_hsValue.add( szKey, szData );
	}
}

void StructPlayer::EnumFlag( std::string & strValueList )
{
	THREAD_SYNCRONIZE( m_csHash );

	std::string strKey;
	std::string strData;

	KHash< std::string, hashPr_string_nocase >::node* pValueNode = NULL;

	bool bQuitFlag = false;
	bQuitFlag = m_hsValue.get_first_node( pValueNode ); 
	while( bQuitFlag )
	{
		strValueList += pValueNode->key.m_pStr;
		strValueList += ":";
		strValueList += pValueNode->value;
		strValueList += "\n";

		bQuitFlag = m_hsValue.get_next_node( pValueNode );
	}
}

// 밑에 Lock처리가 제대로 안 되 있어서 추가했음: IsExistFlag, RemoveFlag (2013-12-17)
const bool StructPlayer::IsExistFlag( const char *szKey ) const
{
	THREAD_SYNCRONIZE( m_csHash );

	return m_hsValue.has( szKey );
}

const bool StructPlayer::RemoveFlag( const char *szKey )
{
	THREAD_SYNCRONIZE( m_csHash );

	return m_hsValue.erase( szKey );
}

std::string StructPlayer::GetAccountFlag( const char *szKey ) const
{
	THREAD_SYNCRONIZE( m_csHashAccount );

	std::string strResult;
	m_hsValueAccount.lookup( szKey, strResult );
	return strResult;
}

void StructPlayer::SetAccountFlag( const char *szKey, const char *szData )
{
	THREAD_SYNCRONIZE( m_csHashAccount );

	if( m_hsValueAccount.has( szKey ) )
	{
		m_hsValueAccount.modify( szKey, szData );
	}
	else
	{
		m_hsValueAccount.add( szKey, szData );
	}
}

const bool StructPlayer::RemoveAccountFlag( const char *szKey )
{
	THREAD_SYNCRONIZE( m_csHashAccount );

	return m_hsValueAccount.erase( szKey );
}

void StructPlayer::EnumAccountFlag( std::string & strValueList )
{
	THREAD_SYNCRONIZE( m_csHashAccount );

	std::string strKey;
	std::string strData;

	KHash< std::string, hashPr_string_nocase >::node* pValueNode = NULL;

	bool bQuitFlag = false;
	bQuitFlag = m_hsValueAccount.get_first_node( pValueNode ); 
	while( bQuitFlag )
	{
		strValueList += pValueNode->key.m_pStr;
		strValueList += ":";
		strValueList += pValueNode->value;
		strValueList += "\n";

		bQuitFlag = m_hsValueAccount.get_next_node( pValueNode );
	}
}

void StructPlayer::DBQuery( GameDBManager::DBProc *pWork )
{
	THREAD_SYNCRONIZE( m_bQueryLock );

	if( m_lQueryList.empty() )
	{
		DB().Push( pWork );
	}

	m_lQueryList.push_back( pWork );
}

void StructPlayer::onEndQuery()
{
	THREAD_SYNCRONIZE( m_bQueryLock );

	m_lQueryList.pop_front();

	if( !m_lQueryList.empty() )
	{
		DB().Push( m_lQueryList.front() );
	}
}

bool StructPlayer::IsTradableWith( StructPlayer * pTarget ) const
{
	// PK 서버면 Demoniac/Bloody 케릭을 노멀 케릭으로 간주한다.
	bool bIsDemoniacOrBloody = ( !GameRule::bIsPKServer && ( IsDemoniacCharacter() || IsBloodyCharacter() ) ) ? true : false;
	bool bIsTargetDemoniacOrBloody = ( !GameRule::bIsPKServer && ( pTarget->IsDemoniacCharacter() || pTarget->IsBloodyCharacter() ) ) ? true : false;

	// 길드원

	if( GetGuildID() && pTarget->GetGuildID() )
	{
		int nAllianceID = GuildManager::GetInstance().GetAllianceID( GetGuildID() );
		if( GetGuildID() == pTarget->GetGuildID() || nAllianceID == GuildManager::GetInstance().GetAllianceID( pTarget->GetGuildID() ) )
		{
			// PKON이 아닌 케릭터끼리이거나 서로 PKON만 가능 -> PKON 상태가 서로 같으면 가능
			if( m_bIsPK != pTarget->m_bIsPK )
				return false;
			else
				return true;
		}
	}
	// 파티원
	if( m_nPartyID && m_nPartyID == pTarget->m_nPartyID )
	{
		// PKON이 아닌 케릭터끼리만 가능
		if( m_bIsPK || pTarget->m_bIsPK )
			return false;
		else
			return true;
	}
	// 관계 없는 사이
	else
	{
		// PKON이 아닌 케릭터끼리만 가능
		if( bIsDemoniacOrBloody || bIsTargetDemoniacOrBloody || m_bIsPK || pTarget->m_bIsPK )
			return false;
		else
			return true;
	}
}

void StructPlayer::StartTrade( StructPlayer * pTarget )
{	
	if( IsTrading() || m_bTradeFreezed ) return;

	clearTradeInfo();
	m_hTradeTarget = pTarget->GetHandle();
}

bool StructPlayer::AddItemToTradeWindow( StructItem * pItem, const __int64 & count )
{
	if( m_bTradeFreezed ) return false;

	if( count < 1 || pItem->GetCount() < count ) return false;

	if( m_vTradeItemList.size() >= GameRule::MAX_TRADE_ITEM_COUNT ) return false;

	std::vector< TradeItemInfo >::iterator it;

	for( it = m_vTradeItemList.begin(); it != m_vTradeItemList.end(); ++it )
	{
		if( (*it).m_hItem == pItem->GetHandle() )
			return false;
	}
	// 소환수 카드는 일단 거래 불가
	// if( pItem->IsSummonCard() ) return;

	m_vTradeItemList.push_back( TradeItemInfo( pItem->GetHandle(), count ) );

	return true;
}

bool StructPlayer::ModifyItemCountInTradeWindow( StructItem * pItem, const __int64 & count )
{
	if( m_bTradeFreezed ) return false;

	if( count < 1 || pItem->GetCount() < count ) return false;

	std::vector< TradeItemInfo >::iterator it;

	for( it = m_vTradeItemList.begin(); it != m_vTradeItemList.end(); ++it )
	{
		if( (*it).m_hItem == pItem->GetHandle() )
		{
			(*it).m_nCount = count;
			return true;
		}
	}

	return false;
}

bool StructPlayer::RemoveItemFromTradeWindow( StructItem * pItem, const __int64 & count )
{
	if( m_bTradeFreezed ) return false;

	if( count < 1 ) return false;

	std::vector< TradeItemInfo >::iterator it;

	for( it = m_vTradeItemList.begin(); it != m_vTradeItemList.end(); ++it )
	{
		if( (*it).m_hItem == pItem->GetHandle() )
		{
			if( (*it).m_nCount <= count )
			{
				m_vTradeItemList.erase( it );
			}
			else
			{
				(*it).m_nCount -= count;

				if( (*it).m_nCount < 1 )
				{
					m_vTradeItemList.erase( it );
				}
			}

			return true;
		}
	}

	return false;
}

bool StructPlayer::RemoveItemByQuittingDeathmatch()
{
	StructItem *pItem;
	size_t idx;
	std::vector< struct StructItem * > pendFreeItemList;
	std::vector< struct StructItem * >::iterator it;

	for( idx = 0; idx < m_Inventory.GetCount(); idx++ )
	{
		pItem = m_Inventory.Get( idx );

		if( pItem->GetOwnerHandle() != GetHandle() || pItem->GetIdx() != idx )
		{
			assert( 0 );
			continue;
		}

		if( !pItem->IsCashItem() && pItem->IsUsableInOnlyDeathmatch() )
		{
			pendFreeItemList.push_back( pItem );
		}
	}

	for( it = pendFreeItemList.begin(); it != pendFreeItemList.end(); it++ ) 
	{
		pItem = PopItem( *it, (*it)->GetCount() );

		if( !pItem )
		{
			assert( 0 );
			continue;
		}

		StructItem::PendFreeItem( pItem );
	}

	return true;
}

void StructPlayer::AddGoldToTradeWindow( const StructGold & gold )
{	
	if( m_bTradeFreezed || gold < StructGold( 0 ) ) return;

	m_nTradeGold = gold;
	if( m_nTradeGold > GetGold() ) m_nTradeGold = GetGold();
}

void StructPlayer::FreezeTrade()
{
	m_bTradeFreezed = true;
}

void StructPlayer::ConfirmTrade()
{
	m_bTradeAccepted = true;
}

bool StructPlayer::CheckTradeItem()
{
	if( m_vTradeItemList.size() > GameRule::MAX_TRADE_ITEM_COUNT )
	{
		assert( 0 );
		return false; // 정상적으로는 들어올 영역이 아님
	}

	std::vector< TradeItemInfo >::iterator it;
	for( it = m_vTradeItemList.begin(); it != m_vTradeItemList.end(); ++it )
	{
		StructItem * pItem = StructItem::FindItem( (*it).m_hItem );
		if( !pItem || (*it).m_nCount < 1 || pItem->GetCount() < (*it).m_nCount || pItem->GetOwnerHandle() != GetHandle() )
			return false;
	}

	return true;
}

bool StructPlayer::ProcessTrade()
{
	if( !GetTradeTarget() || !m_bTradeFreezed ) return false;

	StructPlayer::iterator itTradeTarget = StructPlayer::get( GetTradeTarget() );
	StructPlayer * pTradeTarget = (*itTradeTarget);

	// 루피 처리(오버 플로우 검사를 위해 루피만 먼저 처리)
	{
		if( !pTradeTarget )
			return false;

		StructGold nPrevGold( GetGold() ), nPrevTradeTargetGold( pTradeTarget->GetGold() );

		// 상대방꺼 가져오기
		unsigned short nResult = pTradeTarget->processTradeGold();
		if( nResult != RESULT_SUCCESS )
		{
			if( nResult == RESULT_TOO_MUCH_MONEY )
			{
				SendResult( this, TM_TRADE, RESULT_TOO_MUCH_MONEY, GetHandle() );
				SendResult( pTradeTarget, TM_TRADE, RESULT_TOO_MUCH_MONEY, GetHandle() );
			}

			// 루피 원상 복귀
			if( ChangeGold( nPrevGold ) != RESULT_SUCCESS || pTradeTarget->ChangeGold( nPrevTradeTargetGold ) != RESULT_SUCCESS )
			{
				assert( 0 );
				_cprint( "ChangeGold/ChangeStorageGold Failed: Case[3], Player[%s], Info[Owned(%I64d), Target(%I64d)]\n", GetName(), GetGold().GetRawData(), nPrevGold.GetRawData() );
				FILELOG( "ChangeGold/ChangeStorageGold Failed: Case[3], Player[%s], Info[Owned(%I64d), Target(%I64d)]", GetName(), GetGold().GetRawData(), nPrevGold.GetRawData() );
			}

			return false;
		}

		// 내꺼 주기
		nResult = processTradeGold();
		if( nResult != RESULT_SUCCESS )
		{
			if( nResult == RESULT_TOO_MUCH_MONEY )
			{
				SendResult( this, TM_TRADE, RESULT_TOO_MUCH_MONEY, pTradeTarget->GetHandle() );
				SendResult( pTradeTarget, TM_TRADE, RESULT_TOO_MUCH_MONEY, pTradeTarget->GetHandle() );
			}

			// 루피 원상 복귀
			if( ChangeGold( nPrevGold ) != RESULT_SUCCESS || pTradeTarget->ChangeGold( nPrevTradeTargetGold ) != RESULT_SUCCESS )
			{
				assert( 0 );
				_cprint( "ChangeGold/ChangeStorageGold Failed: Case[4], Player[%s], Info[Owned(%I64d), Target(%I64d)]\n", GetName(), GetGold().GetRawData(), nPrevGold.GetRawData() );
				FILELOG( "ChangeGold/ChangeStorageGold Failed: Case[4], Player[%s], Info[Owned(%I64d), Target(%I64d)]", GetName(), GetGold().GetRawData(), nPrevGold.GetRawData() );
			}

			return false;
		}
	}

	// 아이템 처리(기존대로 오류 처리 안 함 -_ -;)
	// 사실 오류가 발생하면 절대 안 되고, 발생했을 때 롤백도 불가능 함
	{
		unsigned short nTradeTargetResult = pTradeTarget->processTradeItem();
		unsigned short nResult = processTradeItem();
		if( nTradeTargetResult != RESULT_SUCCESS || nResult != RESULT_SUCCESS )
		{
			assert( 0 );
			_cprint( "StructPlayer::ProcessTrade(): Error on trading item %s(%s) with %s(%s). Code(%d)\n",
				GetName(), GetAccountName(), pTradeTarget->GetName(), pTradeTarget->GetAccountName(),
				( nResult != RESULT_SUCCESS ) ? nResult : nTradeTargetResult );
			FILELOG( "StructPlayer::ProcessTrade(): Error on trading item %s(%s) with %s(%s). Code(%d)",
				GetName(), GetAccountName(), pTradeTarget->GetName(), pTradeTarget->GetAccountName(),
				( nResult != RESULT_SUCCESS ) ? nResult : nTradeTargetResult );

			return false;
		}
	}

	pTradeTarget->clearTradeInfo();

	pTradeTarget->Save();

	Save();

	// 트레이드 정보 삭제
	clearTradeInfo();

	return true;
}

bool StructPlayer::CheckTradeWeight()
{
	if( !GetTradeTarget() )
		return false;

	StructPlayer::iterator itTradeTarget = StructPlayer::get( GetTradeTarget() );
	StructPlayer * pTradeTarget = (*itTradeTarget);

	if( !pTradeTarget )
		return false;

	int weight = 0;
	std::vector< TradeItemInfo >::iterator it;
	for( it = m_vTradeItemList.begin(); it != m_vTradeItemList.end(); ++it )
	{
		StructItem * pItem = StructItem::FindItem( (*it).m_hItem );
		if( pItem ) weight += pItem->GetWeight();
	}

	if( pTradeTarget->GetMaxWeight() - pTradeTarget->GetWeight() < weight ) return false;

	return true;
}

unsigned short StructPlayer::processTradeGold()
{
	if( !GetTradeTarget() || !m_bTradeFreezed )
		return RESULT_NOT_ACTABLE;

	StructPlayer::iterator itTradeTarget = StructPlayer::get( GetTradeTarget() );
	StructPlayer * pTradeTarget = (*itTradeTarget);

	if( !pTradeTarget )
		return RESULT_NOT_ACTABLE;

	if( !GetSID() || !strlen( GetName() ) )
	{
		FileLogHandler::GetFileLogHandler()->LogStringEx( NULL, "BugReport", "Use No Character Login Bug [%s]", GetAccountName() );
		GameRule::RegisterBlockAccount( GetAccountName() );
		if( pConnection && pConnection->IsConnected() )
			pConnection->Close();

		return RESULT_NOT_EXIST;
	}

	// 돈 주고
	if( !!m_nTradeGold )
	{
		if( m_nTradeGold < StructGold( 0 ) )
		{
			FileLogHandler::GetFileLogHandler()->LogStringEx( NULL, "BugReport", "Proc Trade gold Bug [%s:%s]", GetAccountName(), GetName() );

			GameRule::RegisterBlockAccount( GetAccountName() );
			if( pConnection && pConnection->IsConnected() )
				pConnection->Close();

			return RESULT_ACCESS_DENIED;
		}

		StructGold nPrevGold( GetGold() );
		if( ChangeGold( GetGold() - m_nTradeGold ) != RESULT_SUCCESS || pTradeTarget->ChangeGold( pTradeTarget->GetGold() + m_nTradeGold ) != RESULT_SUCCESS )
		{
			// 오류 났다면 변경됐을 가능성이 있는 대상은 this 뿐임
			if( ChangeGold( nPrevGold ) != RESULT_SUCCESS )
			{
				assert( 0 );
				_cprint( "ChangeGold/ChangeStorageGold Failed: Case[5], Player[%s], Info[Owned(%I64d), Target(%I64d)]\n", GetName(), GetGold().GetRawData(), nPrevGold.GetRawData() );
				FILELOG( "ChangeGold/ChangeStorageGold Failed: Case[5], Player[%s], Info[Owned(%I64d), Target(%I64d)]", GetName(), GetGold().GetRawData(), nPrevGold.GetRawData() );
			}
			return RESULT_TOO_MUCH_MONEY;
		}

		LOG::Log11N4S( LM_ITEM_TRADE_GIVE,
			GetAccountID(), GetSID(),
			0, 0, 0, 0,
			m_nTradeGold.GetRawData(),
			GetGold().GetRawData(),
			pTradeTarget->GetPlayerUID(),
			0, 0,
			GetAccountName(), LOG::STR_NTS,
			GetName(), LOG::STR_NTS,
			pTradeTarget->GetName(), LOG::STR_NTS,
			"", 0 );

		LOG::Log11N4S( LM_ITEM_TRADE_TAKE,
			pTradeTarget->GetAccountID(), pTradeTarget->GetSID(),
			0, 0, 0, 0,
			m_nTradeGold.GetRawData(),
			pTradeTarget->GetGold().GetRawData(),
			GetPlayerUID(),
			0, 0,
			pTradeTarget->GetAccountName(), LOG::STR_NTS,
			pTradeTarget->GetName(), LOG::STR_NTS,
			GetName(), LOG::STR_NTS,
			"", 0 );
	}

	return RESULT_SUCCESS;
}

unsigned short StructPlayer::processTradeItem()
{
	if( !GetTradeTarget() || !m_bTradeFreezed )
		return RESULT_NOT_ACTABLE;

	StructPlayer::iterator itTradeTarget = StructPlayer::get( GetTradeTarget() );
	StructPlayer * pTradeTarget = (*itTradeTarget);

	if( !GetSID() || !strlen( GetName() ) )
	{
		FileLogHandler::GetFileLogHandler()->LogStringEx( NULL, "BugReport", "Use No Character Login Bug [%s]", GetAccountName() );
		GameRule::RegisterBlockAccount( GetAccountName() );
		if( pConnection && pConnection->IsConnected() )
			pConnection->Close();

		return RESULT_NOT_EXIST;
	}

	// 아이템 수량 체크
	std::vector< TradeItemInfo >::iterator it;
	for( it = m_vTradeItemList.begin(); it != m_vTradeItemList.end(); ++it )
	{
		if( (*it).m_nCount < 0 )
		{
			FileLogHandler::GetFileLogHandler()->LogStringEx( NULL, "BugReport", "Proc Trade item Bug [%s:%s]", GetAccountName(), GetName() );

			GameRule::RegisterBlockAccount( GetAccountName() );
			if( pConnection && pConnection->IsConnected() )
				pConnection->Close();

			return RESULT_ACCESS_DENIED;
		}
	}

	// 아이템 준다(nTradeCount가 음수인 경우나 기타 예외 사항은 위에서 검사 완료)
	for( it = m_vTradeItemList.begin(); it != m_vTradeItemList.end(); ++it )
	{
		__int64 nTradeCount = (*it).m_nCount;
		StructItem *pOriginalItem = StructItem::FindItem( (*it).m_hItem );

		__int64 nResultCount = pOriginalItem->GetCount() - nTradeCount;
		AR_HANDLE hResultItem;

		if( GiveItem( pTradeTarget, pOriginalItem->GetHandle(), nTradeCount, &hResultItem ) )
		{
			GameObject::iterator it = StructItem::get( hResultItem );
			StructItem *pResultItem = static_cast< StructItem * >( *it );

			LOG::Log11N4S( LM_ITEM_TRADE_GIVE,
				GetAccountID(), GetSID(),
				pOriginalItem->GetItemEnhance() * 100 + pOriginalItem->GetItemLevel(),
				pOriginalItem->GetItemCode(),
				nTradeCount,
				pOriginalItem->GetCount(),
				0, 0,
				pTradeTarget->GetPlayerUID(),
				pResultItem->GetItemUID(),
				pOriginalItem->GetItemUID(),
				GetAccountName(), LOG::STR_NTS,
				GetName(), LOG::STR_NTS,
				pTradeTarget->GetName(), LOG::STR_NTS,
				"", 0 );

			LOG::Log11N4S( LM_ITEM_TRADE_TAKE,
				pTradeTarget->GetAccountID(), pTradeTarget->GetSID(),
				pOriginalItem->GetItemEnhance() * 100 + pOriginalItem->GetItemLevel(),
				pOriginalItem->GetItemCode(),
				nTradeCount,
				pResultItem->GetCount(),
				0, 0,
				GetPlayerUID(),
				0,
				pResultItem->GetItemUID(),
				pTradeTarget->GetAccountName(), LOG::STR_NTS,
				pTradeTarget->GetName(), LOG::STR_NTS,
				GetName(), LOG::STR_NTS,
				"", 0 );
		}
	}

	return RESULT_SUCCESS;
}

void StructPlayer::CancelTrade( bool bIsNeedBroadcast )
{
	if( bIsNeedBroadcast ) SendTradeCancelMessage( this );

	clearTradeInfo();
}

void StructPlayer::clearTradeInfo()
{
	m_bTradeAccepted = false;
	m_bTradeFreezed = false;
	m_vTradeItemList.clear();
	m_hTradeTarget = 0;
	m_nTradeGold.SetRawData( 0 );
}

void StructPlayer::EquipSummon( AR_HANDLE* pCardHandle )
{
	int i, j;
	bool bExist[6];
	bool bUnbind[6];
	AR_HANDLE vBindedCard[6];

	bUnbind[0] = bUnbind[1] = bUnbind[2] = bUnbind[3] = bUnbind[4] = bUnbind[5] = true;

	int nCreatureControlLevel = GetCurrentPassiveSkillLevel( StructSkill::SKILL_CREATURE_CONTROL );
	if( nCreatureControlLevel > 6 )
		nCreatureControlLevel = 6;

	// i 위치의 소환수 카드가 클라이언트로부터 요청된 목록인 pCardHandle에 존재하는지 확인하여
	// 존재한다면 유지하고 존재하지 않으면 편성 해제
	for( i = 0; i < nCreatureControlLevel; ++i )
	{
		bExist[i] = false;
		StructItem *pBindedCard = GetSummonCardAt( i );

		if( pBindedCard )
		{
			vBindedCard[i] = pBindedCard->GetHandle();

			// 변경된 편성 목록에서도 대부분의 카드는 제자리를 유지하고 있을 것이므로 현재 위치인 i를 제일 먼저 검사
			// 이를 위해 index 사용
			int index = i;
			for( j = 0; j < nCreatureControlLevel; ++j )
			{
				// 이미 찾은 것은 패스
				if( !bUnbind[index] )
				{
					index = ++index % nCreatureControlLevel;
					continue;
				}

				if( pBindedCard->GetHandle() == pCardHandle[index] )
				{
					bExist[i] = true;
					bUnbind[index] = false;
					break;
				}

				index = ++index % nCreatureControlLevel;
			}

			// 위치만 바뀌는 소환수는 편성 해제하지 않기 때문에 검사하지 않는다.
			if( bExist[i] ) continue;

			// 편성 해제될 소환수에 대한 처리
			// 편성 해제가 실패하면 다른 편성에도 영향을 미치기 때문에 실패 처리한다.
			StructSummon *pSummon = pBindedCard->GetSummonStruct();
			if( pSummon )
			{
				try
				{
					// 소환되어 있는 소환수는 편성이 해제될 수 없다.
					if( pSummon->IsInWorld() )
					{
						throw std::exception();
					}

					// 소환수가 입고 있던 장비 해제
					for( int x= 0; x < ItemBase::MAX_ITEM_WEAR; ++x )
					{
						if( pSummon->GetWearedItem( static_cast< ItemBase::ItemWearType >( x ) ) )
							if( pSummon->Putoff( static_cast< ItemBase::ItemWearType >( x ) ) != RESULT_SUCCESS )
							{
								assert( 0 );
								throw std::exception();
							}
					}

					// 만약 이후에 편성에 실패를 하게 되더라도
					// 일부만이라도 정상적인 편성 해제 처리가 이루어져야 하므로 미리 편성 해제한다.
					SetSummonCardAt( i, NULL );

					// 호칭 : 소환수가 편성되지 않음으로 상태 변경
					UpdateTitleConditionBySummonEquip( pSummon->GetParentCard()->GetSummonCode(), pSummon->GetRate(), pSummon->GetParentCard()->GetItemEnhance(), -1 );
				}
				catch( ... )
				{
					// 클라이언트 입장에서는 편성 실패의 개념이 없으므로 편성 목록을 변경하지 않고 다시 날려줘야 한다.
					SendCreatureEquipMessage( this, false );
					return;
				}
			}
		}
	}

	// 편성
	for( i = 0; i < nCreatureControlLevel; ++i )
	{
		// 만약 아래에서 편성에 실패하게 되면 빈 슬롯으로 남겨둬야 하므로 일단 편성 해제
		SetSummonCardAt( i, NULL );

		if( !pCardHandle[i] ) continue;

		// 이 이하에서 continue; 문이 실행되는 경우는 클라이언트 패킷상으로 편성이 요청된 카드지만 서버에서 체크하기에 편성이 안 되어야 하는 카드인 경우
		// 이 중 이미 편성 되어있던 카드( !bUnbind[i] )는 최초 편성할 때 이미 검사를 했기 때문에 더 이상의 검사를 하지 않는다.

		// 소환수 카드의 내구도의 경우에는 편성된 상태에서도 변경 될 수 있지만 내구도가 모두 소진된 상태로 월드에 소환되어있을 수 있을 뿐더러
		// 편성만 새로 변경하지 않는다면 내구도가 모두 소진된 상태로 편성을 유지할 수 있기 때문에 여기서 편성 해제를 하는 것은 문제의 소지가 될 뿐 큰 의미가 없다.

		// 새로 편성된 소환수( bUnbind[i] )에 대해서만 편성 조건 검사
		StructItem *pItem = StructItem::FindItem( pCardHandle[i] );
		if( bUnbind[i] )
		{
			if( !pItem ) continue;

			// 인벤토리에 없거나, 빈 카드는 편성할 수 없다.
			if( GetHandle() != pItem->GetOwnerHandle() || !pItem->IsInInventory()
				|| !pItem->IsSummonCard() || !pItem->GetInstanceFlag().IsOn( ItemInstance::ITEM_FLAG_SUMMON ) )
				continue;

			bool bBeltSlotEquip = false;
			// 벨트에 장착된 카드는 편성 불가
			for( int bind_idx = 0; bind_idx < 6; ++bind_idx )
			{
				if( GetBeltSlotCardAt( bind_idx ) == pItem )
				{
					bBeltSlotEquip = true;
					break;
				}
			}

			if( bBeltSlotEquip ||																	// 벨트 슬롯에 장착된 카드
				( pItem->GetMaxEtherealDurability() && !pItem->GetCurrentEtherealDurability() ) )	// 내구도가 다 닳은 카드
				continue;

			StructSummon *pSummon = NULL;
			if( !pItem->GetSummonStruct() )
			{
				// 할당
				pSummon = AllocNewSummon( this, pItem );
				pSummon->SetLoginComplete();

				// TS_SC_ADD_SUMMON_INFO 메시지 송신 후에 소환수 카드의 TS_SC_INVENTORY 메시지가 가야 함
				AddSummon( pSummon );

				SendItemMessage( this, pItem );

				std::string strFirstSummonHandler;

				XStringUtil::Format( strFirstSummonHandler, "on_first_summon( %d, %u )", pSummon->GetSummonCode(), pSummon->GetHandle() );

				LUA()->RunString( strFirstSummonHandler.c_str() );

				pSummon->CalculateStat();

				// DB에 기록
				DBQuery( new DB_InsertSummon( this, pSummon->GetSID(), 0, GetPlayerUID(), pSummon->GetSummonCode(), pSummon->GetParentCard()->GetItemUID(), pSummon->GetName(), pSummon->GetSP(), pSummon->GetHP(), pSummon->GetMP(), pSummon->GetJobPoint() ) );
			}
			else
			{
				pSummon = pItem->GetSummonStruct();
			}

			pSummon->SetSummonSlotIndex( i );
			pSummon->CalculateStat();
		}
		
		SetSummonCardAt( i, pItem );
	}

	// 호칭 처리 및 중복제거 (이런짓을 할지도 모르므로 -_-)
	for( int i = 0; i < nCreatureControlLevel; ++i )
	{
		StructItem *pItem = GetSummonCardAt( i );
		if( !pItem ) continue;

		bool bExisted = false;
		for( int y = 0; y < nCreatureControlLevel; ++y )
		{
			if( pItem->GetHandle() == vBindedCard[y] )
				bExisted = true;
		}

		// 호칭 :
		// 호칭 조건을 빈번하게 업데이트하는 것을 피하기 위해
		// 기존에 편성되어있지 않던 소환수에 한해서만 상태 변경
		if( !bExisted )
		{
			StructSummon *pSummon = pItem->GetSummonStruct();
			UpdateTitleConditionBySummonEquip( pSummon->GetParentCard()->GetSummonCode(), pSummon->GetRate(), pSummon->GetParentCard()->GetItemEnhance(), 1 );
		}

		for( int y = i+1; y < nCreatureControlLevel; ++y )
		{
			if( pItem == GetSummonCardAt( y ) ) SetSummonCardAt( y, NULL );
		}
	}

	SetMainAndSubSummon();

	SendCreatureEquipMessage( this, false );
}

// ProcessWarp 함수 안에서 워프 대상 좌표를 변경해야 하는 경우에는 m_nPendWarpX, m_nPendWarpY, m_nPendWarpLayer 변수에 값만 바꾸는 형태로 처리해야 함.
// PendWarp 함수를 호출해도 효과는 비슷하지만 지역 락이 걸리지 않은 채로 GetMain/SubSummon 이 호출되기 때문에 위험할 수 있음
void StructPlayer::ProcessWarp()
{
	bool bWarpToInstanceGame = false;
	unsigned short bWarpToInstanceDungeon = RESULT_SUCCESS;

	// 워프 처리
	if( m_nPendWarpX >= 0 && m_nPendWarpY >= 0 && m_nPendWarpX <= g_nMapWidth && m_nPendWarpY <= g_nMapHeight )
	{
		// 워프하기 전에 인스턴스 던전 안에 있었다가 밖으로 나가는 경우 처리
		int nInstanceDungeonID = InstanceDungeonManager::Instance().GetInstanceDungeonID( GetPos() );
		int nWarpInstanceDungeonID = InstanceDungeonManager::Instance().GetInstanceDungeonID( ArPosition( m_nPendWarpX, m_nPendWarpY ) );

		if( nInstanceDungeonID && nWarpInstanceDungeonID && nInstanceDungeonID != nWarpInstanceDungeonID )
		{
			assert( 0 );
		}
		else if( nInstanceDungeonID && !nWarpInstanceDungeonID )
		{
			InstanceDungeonManager::Instance().LeaveInstanceDungeon( nInstanceDungeonID, this );
		}
		else if( !nInstanceDungeonID && nWarpInstanceDungeonID )
		{
			ArPosition posInInstanceDungeon;
			unsigned char nLayerInInstanceDungeon = 0;

			if( m_bForceWarp == 1 )
			{
				nLayerInInstanceDungeon = m_nPendWarpLayer;
				// 방이 실제로 존재하는 지 검사. 강제 워프는 허용하지만 방은 어쨋든 존재해야한다.
				if( InstanceDungeonManager::Instance().GetInstanceDungeonTypeID( nWarpInstanceDungeonID, m_nPendWarpLayer ) == -1 )
				{
					bWarpToInstanceDungeon = RESULT_NOT_EXIST;
				}
			}
			else
			{
				bWarpToInstanceDungeon = InstanceDungeonManager::Instance().JoinInstanceDungeon( nWarpInstanceDungeonID, this, posInInstanceDungeon, nLayerInInstanceDungeon );

				if( bWarpToInstanceDungeon != RESULT_SUCCESS )
				{
					// 입장에 실패하였고 방 생성에 성공하였을 경우
					bWarpToInstanceDungeon = InstanceDungeonManager::Instance().CreateInstanceDungeon( nWarpInstanceDungeonID, this, m_nPendWarpInstanceDungeonType, 0 );
					if( bWarpToInstanceDungeon == RESULT_SUCCESS )
					{
						// 다시 입장 시도
						bWarpToInstanceDungeon = InstanceDungeonManager::Instance().JoinInstanceDungeon( nWarpInstanceDungeonID, this, posInInstanceDungeon, nLayerInInstanceDungeon );
					}
					else
					{
						if( bWarpToInstanceDungeon == RESULT_LIMIT_MAX )
							SendChatMessage( false, CHAT_NOTICE, "@SYSTEM", this, "@1222" );
						else if( bWarpToInstanceDungeon == RESULT_NOT_ACTABLE_IN_BATTLE_ARENA )
							SendChatMessage( false, CHAT_NOTICE, "@SYSTEM", this, "@2431" );

						// 입장도 실패하고 방 생성도 실패했다면 원 위치로 돌려 보내는 처리를 해줘야 하는데
						// bWarpToInstanceDungeon != RESULT_SUCCESS 에 의해 바로 아래쪽에서 다른 입장 실패 경우랑 묶어서 돌려보내기 처리 됨
					}
				}
			}

			// 최종적으로 입장에 실패하였을 경우 혹은 성공했는데 layer번호가 0 이었다면 돌아가야 할 곳으로 되돌려 보냄
			if( bWarpToInstanceDungeon != RESULT_SUCCESS || !nLayerInInstanceDungeon )
			{
				ArPosition pos;
				GetPositionOnEnterInstanceGame( &pos );

//				if (m_bForceWarp == 0)
//				{
					m_nPendWarpX = pos.x;
					m_nPendWarpY = pos.y;
//				}
				// 이미 워프 전의 인던 ID와 워프 후의 인던 ID가 달랐다는 게 이 부분의 전제 조건이므로
				// 채널ID도 무조건 서로 다름. 따라서 이동하는 대상 위치의 채널이 존재하는 경우에는 그쪽에서 레이어 번호 발급해줘야 함.
				int target_channel = ChannelManager::GetChannelId( pos.x, pos.y );
				if( target_channel )
					m_nPendWarpLayer = ChannelManager::GetProperLayer( pos.x, pos.y );
				else
					m_nPendWarpLayer = 0;
			}
			// 입장에 성공했다면 워프 좌표 조정(인던쪽에서 반환해 준 입장 위치 및 레이어로. 단, 강제 워프 좌표조정 안 한다)
			else if (bWarpToInstanceDungeon == RESULT_SUCCESS)// && !m_bForceWarp )
			{
				if (m_bForceWarp == 0)
				{
					m_nPendWarpX = posInInstanceDungeon.x;
					m_nPendWarpY = posInInstanceDungeon.y;
				}
				m_nPendWarpLayer = nLayerInInstanceDungeon;
			}
			m_bForceWarp = false;
		}

		// 워프하기 전에 헌터홀릭 안에 있었다가 밖으로 나가는 경우 처리
		int nHuntaholicID = HuntaholicManager::Instance().GetHuntaholicID( GetPos() );
		if( nHuntaholicID && HuntaholicManager::Instance().GetHuntaholicID( ArPosition( m_nPendWarpX, m_nPendWarpY ) ) != nHuntaholicID )
		{
			// 헌터홀릭 이탈 시 소멸 지속효과 처리
			{
				ARCADIA_LOCK( ArcadiaServer::Instance().LockObjectWithVisibleRange( this ) );

				RemoveAllStateByQuittingHuntaholic();
				RestoreStatesOnLeaveInstanceGame( true );
			}

			// 가입되어 있던 방이 있으면 탈퇴 처리
			if( IsInParty() )
			{
				if( HuntaholicManager::Instance().IsHuntaholicLobby( GetPos() ) )
				{
					unsigned short nErrorCode = HuntaholicManager::Instance().LeaveInstanceDungeon( nHuntaholicID, this );
					if( nErrorCode != RESULT_SUCCESS && IsInParty() )
					{
						_cprint( "StructPlayer::ProcessWarp(): LeaveInstanceDungeon failed(ErrorCode: %d)\n", nErrorCode );
						FILELOG( "StructPlayer::ProcessWarp(): LeaveInstanceDungeon failed(ErrorCode: %d)", nErrorCode );

						// 강제로 파티만 탈퇴 시킴
						int nPartyID = GetPartyID();
						if( PartyManager::GetInstance().IsLeader( nPartyID, GetPlayerUID() ) )
						{
							if( PartyManager::GetInstance().AutoPromote( nPartyID, true ) )
							{
								PrintfPartyChatMessage( CHAT_PARTY_SYSTEM, nPartyID, "PROMOTE|%d|%s|", nPartyID, PartyManager::GetInstance().GetLeaderDisplayName( nPartyID ).c_str() );
								BroadcastPartyLeave( this );
								PartyManager::GetInstance().LeaveParty( nPartyID, GetPlayerUID() );
							}
							else
							{
								BroadcastPartyDestroy( nPartyID );
								PartyManager::GetInstance().DestroyParty( nPartyID );
							}
						}
						else
							PartyManager::GetInstance().LeaveParty( nPartyID, GetPlayerUID() );
					}
				}
				else
				{
					// QuitHunting은 PendWarp를 동반하기 때문에 워프 좌표를 기억해놨다가 QuitHunting 완료 후 복원시켜야 함.
					ArPosition posWarp( m_nPendWarpX, m_nPendWarpY );
					unsigned char nWarpLayer = m_nPendWarpLayer;

					unsigned short nErrorCode = HuntaholicManager::Instance().QuitHunting( nHuntaholicID, this, false );

					m_nPendWarpX = posWarp.GetX();
					m_nPendWarpY = posWarp.GetY();
					m_nPendWarpLayer = nWarpLayer;

					{
						ARCADIA_LOCK( ArcadiaServer::Instance().LockObjectWithVisibleRange( this ) );

						// 헌터홀릭 사냥 진행 중에 워프를 통해 밖으로 나갈 경우 점수를 받지 못하지만 페널티도 받지 않으므로 페널티를 이 경우에만 별도로 적용
						AddHuntaholicEnterableCount( -1 );
						AR_TIME t = GetArTime();
						AddState( StructState::MOVE_SPEED_SLOWDOWN, GetHandle(), 1, t, t + GameRule::HUNTAHOLIC_QUITTING_PENALTY_DEBUFF_TIME );
					}

					if( nErrorCode != RESULT_SUCCESS && IsInParty() )
					{
						_cprint( "StructPlayer::ProcessWarp(): QuitHunting failed(ErrorCode: %d)\n", nErrorCode );
						FILELOG( "StructPlayer::ProcessWarp(): QuitHunting failed(ErrorCode: %d)", nErrorCode );

						// 강제로 파티만 탈퇴 시킴
						int nPartyID = GetPartyID();
						if( PartyManager::GetInstance().IsLeader( nPartyID, GetPlayerUID() ) )
						{
							if( PartyManager::GetInstance().AutoPromote( nPartyID, true ) )
							{
								PrintfPartyChatMessage( CHAT_PARTY_SYSTEM, nPartyID, "PROMOTE|%d|%s|", nPartyID, PartyManager::GetInstance().GetLeaderDisplayName( nPartyID ).c_str() );
								BroadcastPartyLeave( this );
								PartyManager::GetInstance().LeaveParty( nPartyID, GetPlayerUID() );
							}
							else
							{
								BroadcastPartyDestroy( nPartyID );
								PartyManager::GetInstance().DestroyParty( nPartyID );
							}
						}
						else
							PartyManager::GetInstance().LeaveParty( nPartyID, GetPlayerUID() );
					}
				}

				// 방 정보 UI 없애기 위해서 날로 RESULT 메시지 하나 보냄
				SendResult( this, TM_CS_HUNTAHOLIC_LEAVE_INSTANCE, RESULT_SUCCESS );
			}
		}
		else if( !nHuntaholicID && HuntaholicManager::Instance().GetHuntaholicID( ArPosition( m_nPendWarpX, m_nPendWarpY ) ) )
			bWarpToInstanceGame = true;

		int nDungeonID = DungeonManager::Instance().GetDungeonID( GetPos().GetX(), GetPos().GetY() );
		int nWarpDungeonID = DungeonManager::Instance().GetDungeonID( m_nPendWarpX, m_nPendWarpY );

		// 던전 내에 있는 상태에서 레이드가 시작하여 같은 던전 ID를 가졌음에도 불구하고 레이어만 바뀌어 워프하는 경우가 발생한다.
		// 따라서 중복 방송되더라도 일반 레이어가 아니라면 무조건 방송하도록 변경.
		if( nWarpDungeonID )
		{
			if( m_nPendWarpLayer == DungeonManager::DUNGEON_SIEGE_LAYER )
			{
				int nGuildID = GetGuildID();
				if( GuildManager::GetInstance().GetAllianceID( nGuildID ) )
				{
					nGuildID = GuildManager::GetInstance().GetAllianceLeaderGuildID( GuildManager::GetInstance().GetAllianceID( nGuildID ) );
				}

				// 던전을 소유하여 방어자 길드일 경우 1, 그렇지 않아 공격자 길드일 경우 0
				int nPosition = ( nGuildID == DungeonManager::Instance().GetOwnGuildID( nWarpDungeonID ) );
				
				if( nPosition )
					PrintfChatMessage( false, CHAT_RAID_SYSTEM, "@RAID", this, "SIEGE_OPPONENT|%s|", GuildManager::GetInstance().GetGuildName( DungeonManager::Instance().GetRaidGuildID( nWarpDungeonID ) ).c_str() );
				else
					PrintfChatMessage( false, CHAT_RAID_SYSTEM, "@RAID", this, "SIEGE_OPPONENT|%s|", GuildManager::GetInstance().GetGuildName( DungeonManager::Instance().GetOwnGuildID( nWarpDungeonID ) ).c_str() );

				PrintfChatMessage( false, CHAT_RAID_SYSTEM, "@RAID", this, "SIEGE_STATUS|%d|%d|%d|", DungeonManager::Instance().GetConnectorHP( nWarpDungeonID ), DungeonManager::Instance().GetDungeonCoreHP( nWarpDungeonID ), nPosition );

				// 공격자인지 방어자인지는 원래 던전 소유길드를 기준으로 판단되어야 한다.
				UpdateTitleConditionByDungeonSiegeStart( nWarpDungeonID, nGuildID != DungeonManager::Instance().GetOriginalOwnGuildID( nWarpDungeonID ) );
			}
			else if( m_nPendWarpLayer > DungeonManager::DUNGEON_SIEGE_LAYER )
			{
				PrintfChatMessage( false, CHAT_RAID_SYSTEM, "@RAID", this, "RAID_ENTER|%d|", DungeonManager::Instance().GetElaspedRaidTime( nWarpDungeonID, m_nPendWarpLayer ) );
			}
		}

		bool bWasInDeathmatch = IsInDeathmatch();
#ifdef _DEBUG
		if( !bWasInDeathmatch )
		{
			int nChannelID = ChannelManager::GetChannelId( GetX(), GetY() );
			if( ( 120501 <= nChannelID && nChannelID <= 120504 ) || ( 120601 <= nChannelID && nChannelID <= 120604 ) )
			{
				_cprint( "Player is in deathmatch channel but the world location info does not match(Warp)[%f,%f ChannelID:%d].\n", GetX(), GetY(), nChannelID );
				FILELOG( "Player is in deathmatch channel but the world location info does not match(Warp)[%f,%f ChannelID:%d].", GetX(), GetY(), nChannelID );
			}
		}
#endif

		bool bWasInBattleArena = IsInBattleArena();

		unsigned min_rx = unsigned(-1);
		unsigned max_rx = 0;
		unsigned min_ry = unsigned(-1);
		unsigned max_ry = 0;

		{
			ARCADIA_LOCK( ArcadiaServer::Instance().LockObjectWithVisibleRange( this ) );

			for( std::vector< StructSummon* >::iterator it = m_vSummonList.begin(); it != m_vSummonList.end(); ++it )
			{
				if( (*it)->GetSummonFlag() && (*it)->IsInWorld() )
				{
					if( (*it)->GetRX() < min_rx )
					{
						min_rx = (*it)->GetRX();
					}
					if( (*it)->GetRX() > max_rx )
					{
						max_rx = (*it)->GetRX();
					}

					if( (*it)->GetRY() < min_ry )
					{
						min_ry = (*it)->GetRY();
					}
					if( (*it)->GetRY() > max_ry )
					{
						max_ry = (*it)->GetRY();
					}
				}
			}
		}

		unsigned rx = GetRX();
		unsigned ry = GetRY();
		short layer = GetLayer();
		short lock_layer = ( layer == m_nPendWarpLayer ) ? layer : -1;

		unsigned min_rx_value = std::min( std::min( rx, GetRegionX( m_nPendWarpX ) ), min_rx );
		unsigned max_rx_value = std::max( std::max( rx, GetRegionX( m_nPendWarpX ) ), max_rx );

		unsigned min_ry_value = std::min( std::min( ry, GetRegionY( m_nPendWarpY ) ), min_ry );
		unsigned max_ry_value = std::max( std::max( ry, GetRegionY( m_nPendWarpY ) ), max_ry );

		ArcadiaLock __lock = ArcadiaServer::Instance().LockArea( min_rx_value, min_ry_value, max_rx_value, max_ry_value, lock_layer );

		while( rx != GetRX() || ry != GetRY() || layer != GetLayer() )
		{
			ArcadiaServer::Instance().UnLock( &__lock );

			rx = GetRX();
			ry = GetRY();
			layer = GetLayer();
			lock_layer = ( layer == m_nPendWarpLayer ) ? layer : -1;

			unsigned min_rx_value = std::min( std::min( rx, GetRegionX( m_nPendWarpX ) ), min_rx );
			unsigned max_rx_value = std::max( std::max( rx, GetRegionX( m_nPendWarpX ) ), max_rx );

			unsigned min_ry_value = std::min( std::min( ry, GetRegionY( m_nPendWarpY ) ), min_ry );
			unsigned max_ry_value = std::max( std::max( ry, GetRegionY( m_nPendWarpY ) ), max_ry );

			__lock = ArcadiaServer::Instance().LockArea( min_rx_value, min_ry_value, max_rx_value, max_ry_value, lock_layer );
		}

		ARCADIA_LOCK( __lock );

		if( !IsInWorld() )
			return;

		// 배틀 아레나 경기장 외부에서 안으로 워프하는 경우에 BattleArenaManager::onProcess에서 입장을 위해 PendWarp는 시켰지만,
		// 그 후에 QuitGame이 호출됐거나 경기에서 이탈된 경우라면 입장 워프 처리 취소(정확한 체크를 위해 지역락 걸고 나서 처리)
		if( m_nPendWarpLayer > 0 && !bWasInBattleArena )
		{
			if( WorldLocationManager::Instance().GetLocationType( GameContent::GetLocationId( m_nPendWarpX, m_nPendWarpY ) ) == StructWorldLocation::LOCATION_BATTLE_ARENA &&
				BattleArenaManager::Instance().GetTeamNo( this ) == INVALID_BATTLE_ARENA_TEAM_NO )
			{
				m_nPendWarpX = m_nPendWarpY = -1;
				m_nPendWarpLayer = 0;
				return;
			}
		}

		// 기존 위치에서 제거
		WarpBegin( this );		

		// 새 위치에 생성
		WarpEnd( this, &ArPosition( m_nPendWarpX, m_nPendWarpY, 0), m_nPendWarpLayer );

		if( IsInDeathmatch() )
		{
			// 워프하기 전에 매스매치 밖에 있었다가 안으로 들어오는 경우 처리
			if( !bWasInDeathmatch )
			{
				bWarpToInstanceGame = true;

				RemoveStateByEnteringDeathmatch();
				if( GetMainSummon() )
					GetMainSummon()->RemoveStateByEnteringDeathmatch();
				if( GetSubSummon() )
					GetSubSummon()->RemoveStateByEnteringDeathmatch();
			}

			// 무조건 데스매치 안으로 워프될 경우 HP회복하고 무적 버프
			AddHP( GetMaxHP() );
			AddMP( GetMaxMP() );

			Save();

			BroadcastHPMPMsg( this, GetHP(), GetMP() );

			AR_TIME t = GetArTime();
			AddState( StructState::PROTECTING_FORCE_OF_BEGINNING, 0, 1, t, t + GameRule::BEGINNING_TIME_IN_DEATHMATCH );
		}
		else 
		{
			// 워프하기 전에 데스매치 안에 있었다가 밖으로 나가는 경우 처리
			if( bWasInDeathmatch )
			{
				RemoveAllStateByQuittingDeathmatch();
				RemoveItemByQuittingDeathmatch();

				if( GetMainSummon() )
					GetMainSummon()->RemoveAllStateByQuittingDeathmatch();
				if( GetSubSummon() )
					GetSubSummon()->RemoveAllStateByQuittingDeathmatch();

				RestoreStatesOnLeaveInstanceGame( true );
			}
		}

		// 배틀 아레나 안으로 새로 진입한 경우
		if( !bWasInBattleArena )
		{
			if( IsInBattleArena() )
				// 자신의 소속 팀에 따른 StatusCode가 변경되어 클라에서 피아판정에 사용해야 하기 때문에 방송
				SendStatusMessage( this, this );
		}
		// 배틀 아레나에서 이탈하는 경우(이탈하는 워프를 하는데 경기에 참여중이었다면 허용되지 않은 이탈)
		else if( !IsInBattleArena() && GetBattleArenaID() )
		{
			// 경기에서 자의로 이탈하는 것으로 처리
			unsigned short nResult = RESULT_SUCCESS;
			if( GetBattleArenaID() )
				// QuitGame 안에서 BattleArenaManager::BattleArenaQuitFunctor가 동작하게 되지만
				// 이미 워프 처리해서 밖으로 빠져 나온 상태이므로 아무 영향 없음
				nResult = BattleArenaManager::Instance().QuitGame( this, false, false, ALT_LEAVE_ARENA );

			// nResult == RESULT_SUCCESS 라면 이미 관련 패킷들이 방송되었을 거라서 그 외의 경우에만 방송 처리
			if( nResult != RESULT_SUCCESS )
				SendResult( this, TM_CS_BATTLE_ARENA_LEAVE, nResult );
		}

		// 던전에서 던전 내로 워프할 때 굳이 지속효과를 풀었다가 걸지 않는다.
		// 만약 추후에 반드시 매 입퇴장 시에 처리해야 할 내용이 추가된다면 nDungeonID와 nWarpDungeonID를 비교하는 조건을 제외하자.
		if( GetGuildID() && nDungeonID != nWarpDungeonID )
		{
			if( nDungeonID && !layer )
			{
				DungeonManager::Instance().OnExitDungeon( nDungeonID, this, GetGuildID(), layer );
			}

			if( nWarpDungeonID && !m_nPendWarpLayer )
			{
				DungeonManager::Instance().OnEnterDungeon( nWarpDungeonID, this, m_nPendWarpLayer );
			}
		}

		m_nPendWarpX = m_nPendWarpY = -1;
		m_nPendWarpLayer = 0;
		m_nPendWarpInstanceDungeonType = -1;

		m_nWarpEndTime = GetArTime();

		// 워프 이전에 열려있던 다이얼로그 창을 사용할 수 없도록 하기 위해 트리거 리셋
		ClearDialogMenu();
	}

	if( bWarpToInstanceGame )
	{
		// 파티 강제 탈퇴/해산 처리.. 애초에 여기서 파티가 있다는 것 자체가 TS_CS_INSTANCE_GAME_ENTER 패킷을 안 거쳐서 들어온거란 말인데 어쨋든 이벤트로 그럴수도 있으니까 처리함.
		int nPartyID = GetPartyID();
		if( nPartyID )
		{
			if( PartyManager::GetInstance().GetMemberCount( nPartyID ) == 1 )
			{
				BroadcastPartyDestroy( nPartyID );

				LOG::Log11N4S( LM_PARTY_DESTROY, GetAccountID(), GetSID(), nPartyID, GetX(), GetY(), GetLayer(), 5, 0, 0, 0, 0,
					GetAccountName(), LOG::STR_NTS, GetName(), LOG::STR_NTS, PartyManager::GetInstance().GetPartyName( nPartyID ).c_str(), LOG::STR_NTS, "", 0 );

				PartyManager::GetInstance().DestroyParty( nPartyID );
			}
			else
			{
				if( PartyManager::GetInstance().IsLeader( nPartyID, GetPlayerUID() ) )
				{
					if( !PartyManager::GetInstance().AutoPromote( nPartyID, false ) )
					{
						// 리더이고 파티원이 2명 이상인데 자동 승격이 안된다 -> 이 로직에 무슨 문제가 있다.
						assert( 0 );
						return;
					}

					PrintfPartyChatMessage( CHAT_PARTY_SYSTEM, nPartyID, "PROMOTE|%d|%s|",
						nPartyID, PartyManager::GetInstance().GetLeaderDisplayName( nPartyID ).c_str() );
				}

				BroadcastPartyLeave( this );

				LOG::Log11N4S( LM_PARTY_LEAVE, GetAccountID(), GetSID(), nPartyID, GetX(), GetY(), GetLayer(), 3, 0, 0, 0, 0,
					GetAccountName(), LOG::STR_NTS, GetName(), LOG::STR_NTS, PartyManager::GetInstance().GetPartyName( nPartyID ).c_str(), LOG::STR_NTS, "", 0 );

				PartyManager::GetInstance().LeaveParty( nPartyID, GetPlayerUID() );
			}
		}
	}
}

void StructPlayer::onProcess( int nThreadIdx )
{
	ThreadPlayerHelper TPHelper( this );

	if( !IsInWorld() )
		return;

	if ( GetLogoutTimer() && ( GetLogoutTimer() < GetArTime() ) )
	{
		LogoutNowWithAccount( 6 );
		return;
	}

	char buf[255];
	s_sprintf( buf, _countof( buf ), "thread.scheduler.%d.proc", nThreadIdx );
	ENV().Set( buf, "StructPlayer" );

	SchedulingPerformanceHelper helper;
	if( GameRule::bLogSchedulingStatus )
	{
		helper.start();
	}

	AR_TIME t = GetArTime();
	s_sprintf( s_ThreadInfo.job_info, _countof( s_ThreadInfo.job_info ), "StructPlayer(0x%08X)", (UINT_PTR)this );
	s_ThreadInfo.last_execute_time = t;

	AR_TIME nPrevProcessTime = m_nLastProcessTime;

	if( IsPendWarp() )
	{
		ProcessWarp();
		m_nLastProcessTime = t;
	}

	bool bIsMoving = false;
	bool bIsUsingSkill = false;
	bool bIsAttacking = false;

	{
		ARCADIA_LOCK( ArcadiaServer::Instance().LockObjectWithVisibleRange( this ) );

		bIsMoving = IsMoving( t );
		bIsUsingSkill = IsUsingSkill();
		bIsAttacking = IsAttacking();
	}
	//mv.SetMoving(bIsMoving);

	if( !bIsMoving && IsActable() && ( bIsUsingSkill || bIsAttacking ) )
	{
		onAttackAndSkillProcess();
		m_nLastProcessTime = t;
	}

	{
		ARCADIA_LOCK( ArcadiaServer::Instance().LockObjectWithVisibleRange( this ) );

		if( HasPendingMove() )
		{
			processPendingMove();
			m_nLastProcessTime = t;
		}
		else if( IsInvincible() )
		{
			if( m_nWarpEndTime + INVINCIBLE_DURATION < t )
			{
				SetInvincible( false );

				if( GetMainSummon() )
				{
					GetMainSummon()->SetInvincible( false );
				}

				if( GetSubSummon() )
				{
					GetSubSummon()->SetInvincible( false );
				}
			}
		}
		else if( !IsAttacking() && !IsUsingSkill() && !bIsMoving )	
		{
			if( m_nLastProcessTime == 0 )
			{
				m_nLastProcessTime = t;
			}
			else if( m_nLastProcessTime + 500 < t && m_nLastStateProcTime + 500 < t )
			{
				// 아무것도 하지 않는 유저에 대해 PRIORITY를 낮추는 처리이다.
				// 그러나 PRIORITY_LOW로 낮추는 것은 오오라(결속 등)를 균일하게 적용받지 못하는 등의 문제가 있을 수 있다.
				// 따라서 PRIORITY_NORMAL로 세팅하도록 변경한다. (PRIORITY_NORMAL에서도 별 문제가 없다고 판단.)
				ArcadiaServer::Instance().SetObjectPriority( this, ArSchedulerObject::UPDATE_PRIORITY_NORMAL );

				m_nLastProcessTime = 0;
			}

		}

		// 무저갱(오토 수용소) 밖이면 1/10000 확률로 수용소로 송환
		if( GetLocationId() != StructWorldLocation::LOCATION_ID_ABADON && XRandom(0, 10000) == 0 )
		{
			ThreadPlayerHelper TPHelper( this );
			LUA()->RunString("kick_auto_to_another_world()");
		}
	}

	// 역소환 대기 중인 소환수/펫이 있다면 역소환(지역 락 걸고하면 데드락 걸림)
	{
		procPendingUnSummon();
		procPendingUnSummonPet();
	}

	// 게임 중독 방지 시스템이 적용되어 있는 경우에 헌터홀릭에서 강제 탈퇴 시킴
	if( IsGameTimeLimited() && GetContinuousPlayTime() >= GameRule::nMaxTiredGameTime )
	{
		int nHuntaholicID = HuntaholicManager::Instance().GetHuntaholicID( GetPos() );

		if( nHuntaholicID && GetPartyID() )
		{
			unsigned short nErrorCode = RESULT_SUCCESS;

			if( HuntaholicManager::Instance().IsHuntaholicLobby( GetPos() ) )
			{
				nErrorCode = HuntaholicManager::Instance().LeaveInstanceDungeon( nHuntaholicID, this );
			}
			else
			{
				nErrorCode = HuntaholicManager::Instance().QuitHunting( nHuntaholicID, this );
			}

			// 클라 UI 없애는 처리가 진행되도록 유저가 취소 버튼 클릭한 것과 같은 응답 패킷을 보내 줌
			SendResult( this, TM_CS_HUNTAHOLIC_LEAVE_INSTANCE, nErrorCode, nHuntaholicID );
		}
	}

	// 헌터홀릭 입장 횟수 재충전 체크
	time_t tCurrent = time( NULL );
	if( tCurrent >= m_tNextHuntaholicEnterableCountRefill )
	{
		SetHuntaholicEnterableCount( GameRule::HUNTAHOLIC_ENTERANCE_COUNT_PER_DAY );
		Save( true );

		// 내일 충전 시점 세팅
		// DST 문제를 피하기 위해 tm 값을 세팅하고 local-time에서 다시 time_t 값을 계산함(DST 적용/해제 시 기존 시간 + 86400을 하면 어긋나는 경우가 발생함)
		struct tm tmRefill;
		errno_t nError = localtime_s( &tmRefill, &tCurrent );
		if( !nError )
		{
			++tmRefill.tm_mday;
			tmRefill.tm_hour = 6;
			tmRefill.tm_min = 0;
			tmRefill.tm_sec = 0;
			tmRefill.tm_isdst = -1;

			time_t tRefill = mktime( &tmRefill );
			if( tRefill == -1 )
				m_tNextHuntaholicEnterableCountRefill += 86400;
			else
				m_tNextHuntaholicEnterableCountRefill = tRefill;
		}
		else
			m_tNextHuntaholicEnterableCountRefill += 86400;
	}

	// 배틀 아레나 페널티 중첩 카운트 감소 체크
	// * BattleArenaManager 에서 캐릭터 페널티를 증가시키는 처리와 동시에 일어나면 데이터가 꼬일 수 있음;;
	//   근데 이거 하나 보호하자고 락을 따로 달아주는 건 무리고, 지역락으로 보호하는 건 불가능하고... 뭐, 좀 그르타;; 일단 방치 -_ -;
	//   (참고로 베어로드 입장 횟수도 이런 식이지만 별 일 없고, 페널티 중첩 카운트 감소가 그렇게 자주 일어나는 일도 아님 ` `;)
	//   문제가 되는 상황이 발생하면 StructPlayer에 XCriticalSection이나 XSpinLock 하나 추가하고
	//   여기랑 BattleArenaManager 두 군데서 락을 걸고 배틀 아레나 관련 페널티를 처리하도록 하면 됨
	{
		int nPenaltyCount = GetBattleArenaPenaltyCount();
		time_t tPenaltyDecrease = GetBattleArenaPenaltyDecTime();
		bool bPenaltyDecreased = false;
		while( nPenaltyCount > 0 && tCurrent >= tPenaltyDecrease )
		{
			bPenaltyDecreased = true;

			// 페널티 중첩 카운트를 1 감소시키고, 남은 중첩 카운트에 해당하는 페널티 감소 기간 후 시점으로 다시 감소 시점을 세팅
			--nPenaltyCount;
			SetBattleArenaPenaltyCount( nPenaltyCount );
			if( nPenaltyCount > 0 )
			{
				tPenaltyDecrease += GameRule::GetBattleArenaPenaltyDuration( nPenaltyCount );
				SetBattleArenaPenaltyDecTime( tPenaltyDecrease );
			}
			else
				SetBattleArenaPenaltyDecTime( 0 );
		}

		// 페널티 정보가 변경됐으면 DB에 저장하고 클라에게 방송
		if( bPenaltyDecreased )
		{
			DBQuery( new DB_UpdateBattleArenaPenalty( this ) );
			BattleArenaManager::SendBattleArenaPenaltyMessage( this );
		}
	}

	// world buff
	std::vector< PendedWorldState > vPendedWorldStateList;
	{
		ARCADIA_LOCK( ArcadiaServer::Instance().LockObjectWithVisibleRange( this ) );
		vPendedWorldStateList.swap( m_vPendedWorldStateList );
	}

	if( !vPendedWorldStateList.empty() )
	{
		for( std::vector< PendedWorldState >::iterator i = vPendedWorldStateList.begin(), e = vPendedWorldStateList.end(); i != e; ++i )
		{
			CastWorldState( (StructState::StateCode)i->code, i->level, i->time, i->cooltime, i->itemcode );
		}
	}

	// party buff
	std::vector< PendedWorldState > vPendedPartyStateList;
	{
		ARCADIA_LOCK( ArcadiaServer::Instance().LockObjectWithVisibleRange( this ) );
		vPendedPartyStateList.swap( m_vPendedPartyStateList );
	}

	if( !vPendedPartyStateList.empty() )
	{
		for( std::vector< PendedWorldState >::iterator i = vPendedPartyStateList.begin(), e = vPendedPartyStateList.end(); i != e; ++i )
		{
			CastPartyState( (StructState::StateCode)i->code, i->level, i->time, i->cooltime, i->itemcode );
		}
	}

	// guild buff
	std::vector< PendedWorldState > vPendedGuildStateList;
	{
		ARCADIA_LOCK( ArcadiaServer::Instance().LockObjectWithVisibleRange( this ) );
		vPendedGuildStateList.swap( m_vPendedGuildStateList );
	}

	if( !vPendedGuildStateList.empty() )
	{
		for( std::vector< PendedWorldState >::iterator i = vPendedGuildStateList.begin(), e = vPendedGuildStateList.end(); i != e; ++i )
		{
			CastGuildState( (StructState::StateCode)i->code, i->level, i->time, i->cooltime, i->itemcode );
		}
	}

	StructCreature::onProcess( nThreadIdx );
	if( GameRule::bLogSchedulingStatus )
	{
		helper.end();
		g_PlayerPerformanceTracker.addUpThisResult( helper );
	}
}

void StructPlayer::onCantAttack( AR_HANDLE target, AR_TIME t )
{
	if( !IsMoving() && m_nLastCantAttackTime + 100 < t )
	{
		m_nLastCantAttackTime = t;

		SendCantAttackMsg( this, GetHandle(), target, RESULT_TOO_FAR );
	}
}

void StructPlayer::GetLastTownPosition( ArPosition * pPos ) const
{
	// 스크립트상 귀환좌표는 rx, ry 임.
	pPos->x = atoi( GetFlag( "rx" ).c_str() );
	pPos->y = atoi( GetFlag( "ry" ).c_str() );

	if( !pPos->x || !pPos->y )
	{
		// 이게 쓰이면 뭔가 잘못된 거긴 한데...
		switch( GetRace() )
		{
		case 4:
			pPos->x = 6625 + XRandom( 0, 100 );
			pPos->y = 6980 + XRandom( 0, 100 );
			break;
		case 5:
			pPos->x = 116799 + XRandom( 0, 100 );
			pPos->y = 58205 + XRandom( 0, 100 );
			break;
		default:
			pPos->x = 153513 + XRandom( 0, 100 );
			pPos->y = 77203 + XRandom( 0, 100 );
			break;
		}
	}
}

void StructPlayer::StoreCurrentStatesOnEnterInstanceGame( bool bIncludeHPMP )
{
	// 입장 지점 보관: 스크립트상 인스턴스 게임 진입 위치는 hx, hy 로 사용 함.
	// 진입할 때 스크립트에서 hx, hy를 설정하고 인스턴스 게임을 나갈 때 해당 값을 사용하여 진입 지점으로 이동시켜 줌.
	ArPosition posCurrent = GetCurrentPosition( GetArTime() );
	std::string strFlagBuffer;

	XStringUtil::Format( strFlagBuffer, "%f", posCurrent.GetX() );
	SetFlag( "hx", strFlagBuffer.c_str() );
	XStringUtil::Format( strFlagBuffer, "%f", posCurrent.GetY() );
	SetFlag( "hy", strFlagBuffer.c_str() );

	// 입장 당시 HP/MP 보관
	if( bIncludeHPMP )
	{
		XStringUtil::Format( strFlagBuffer, "%d", GetHP() );
		SetFlag( "hhp", strFlagBuffer.c_str() );
		XStringUtil::Format( strFlagBuffer, "%d", GetMP() );
		SetFlag( "hmp", strFlagBuffer.c_str() );
	}
}

void StructPlayer::GetPositionOnEnterInstanceGame( ArPosition * pPos ) const
{
	if( IsExistFlag( "hx" ) && IsExistFlag( "hy" ) && !HuntaholicManager::Instance().GetHuntaholicID( GetPos() ) )
	{
		pPos->x = atoi( GetFlag( "hx" ).c_str() );
		pPos->y = atoi( GetFlag( "hy" ).c_str() );
	}
	// 설정 된 적 없거나 헌터홀릭에서 나가는 거면 귀환 지점으로 반환해 줌
	else
		GetLastTownPosition( pPos );
}

void StructPlayer::RestoreStatesOnLeaveInstanceGame( bool bIncludeHPMP )
{
	if( bIncludeHPMP && IsExistFlag( "hhp" ) && IsExistFlag( "hmp" ) )
	{
		int nPrevHP = GetHP();
		int nPrevMP = GetMP();

		SetHP( atoi( GetFlag( "hhp" ).c_str() ) );
		SetMP( atoi( GetFlag( "hmp" ).c_str() ) );

		BroadcastHPMPMsg( this, this->GetHP() - nPrevHP, this->GetMP() - nPrevMP );
	}

	RemoveFlag( "hx" );
	RemoveFlag( "hy" );
	RemoveFlag( "hhp" );
	RemoveFlag( "hmp" );
}

void StructPlayer::OpenStorage( bool bStart )
{
	if( IsUsingSkill() )
		return;

	if( !m_bIsStorageRequested )
	{
		bool bReload = m_bIsStorageLoaded;
		m_bIsStorageRequested = true;
		m_bIsStorageLoaded = false;

		openStorage();
		DBQuery( new DB_ReadStorageList( this, bReload ) );
		m_nIsUsingStorage = LOADING_STORAGE;
		return;
	}

	if( m_bIsStorageLoaded ) 
	{
		if( bStart == true )
		{
			openStorage();
			m_nIsUsingStorage = OPEN_STORAGE;
		}
		else
		{
			SendPropertyMessage( this, GetHandle(), "storage_gold", GetStorageGold().GetRawData() );
		}

		if( m_nIsUsingStorage == LOADING_STORAGE )
		{
			m_nIsUsingStorage = OPEN_STORAGE;
		}
	}
}

void StructPlayer::openStorage()
{
	SendOpenStorageMessage( this );
}

const int StructPlayer::GetEventAreaEnterCount( const int nEventAreaID ) const
{
	const int * pnEnterCount = m_hsEventAreaEnterCount.lookup( nEventAreaID );

	return ( pnEnterCount ) ? *pnEnterCount : 0;
}

void StructPlayer::IncEventAreaEnterCount( const int nEventAreaID )
{
	int * pnEnterCount = m_hsEventAreaEnterCount.lookup( nEventAreaID );

	if( pnEnterCount )
	{
		int nNewCount = (*pnEnterCount) + 1;
		// 오버플로우 체크
		if( nNewCount < (*pnEnterCount) )
		{
			assert( 0 );
			return;
		}

		m_hsEventAreaEnterCount.modify( nEventAreaID, nNewCount );
		DBQuery( new DB_UpdateEventAreaEnterCount( this, nEventAreaID, nNewCount ) );
	}
	else
	{
		m_hsEventAreaEnterCount.add( nEventAreaID, 1 );
		DBQuery( new DB_InsertEventAreaEnterCount( this, nEventAreaID, 1 ) );
	}
}

void StructPlayer::SetEventAreaEnterCount( const int nEventAreaID, const int nEnterCount )
{
	int * pnEnterCount = m_hsEventAreaEnterCount.lookup( nEventAreaID );

	if( pnEnterCount )
	{
		m_hsEventAreaEnterCount.modify( nEventAreaID, nEnterCount );

		// 로그인으로 인한 로딩으로 세팅하는 경우 DB에 저장하지 않음
		if( IsLoginComplete() )
			DBQuery( new DB_UpdateEventAreaEnterCount( this, nEventAreaID, nEnterCount ) );
	}
	else
	{
		m_hsEventAreaEnterCount.add( nEventAreaID, nEnterCount );

		// 로그인으로 인한 로딩으로 세팅하는 경우 DB에 저장하지 않음
		if( IsLoginComplete() )
			DBQuery( new DB_InsertEventAreaEnterCount( this, nEventAreaID, nEnterCount ) );
	}
}

void StructPlayer::InsertEventAreaID( const int nEventAreaID )
{
	m_stEventAreaID.insert( nEventAreaID );
}

bool StructPlayer::DeleteEventAreaID( const int nEventAreaID )
{
	if( m_stEventAreaID.find( nEventAreaID ) == m_stEventAreaID.end() )
		return false;

	m_stEventAreaID.erase( nEventAreaID );
	return true;
}

const int RefreshRemainTimeForWorldStateCast( const StructState::StateCode code, const AR_TIME cooltime )
{
	time_t currentUnixTime = time( NULL );
	time_t lastUnixTime = 0;

	char keyString[64];
	s_sprintf( keyString, _countof( keyString ), "state.%d.last_cast_time", code );

	const char* lastTimeString = GVM().Get( keyString );
	int successfullyParsed = 0;

	if( lastTimeString != NULL )
	{
		successfullyParsed = sscanf_s( lastTimeString, "%d", &lastUnixTime );
	}

	int remainTime = lastUnixTime + cooltime / 100 - currentUnixTime;

	if( successfullyParsed != 1 || remainTime < 0 )
	{
		char currentTimeString[64];
		s_sprintf( currentTimeString, _countof( currentTimeString ), "%d", currentUnixTime );
		GVM().Set( keyString, currentTimeString );
	}

	return std::max( remainTime, 0 );
}

const bool StructPlayer::PendWorldState( const StructState::StateCode code, const int level, const AR_TIME time, const AR_TIME cooltime, const int itemcode )
{
	const StateInfo* info = GameContent::GetStateInfo( code );

	if( info == NULL )
	{
		return false;
	}

	int remainTime = RefreshRemainTimeForWorldStateCast( code, cooltime );

	if( remainTime > 0 )
	{
		PrintfChatMessage( true, CHAT_ITEM, "@SYSTEM",  this, "@2931\v#@world_buff_cooltime@#\v%d:%d", remainTime / 60, remainTime % 60 );
		return false;
	}
	else
	{
		m_vPendedWorldStateList.push_back( PendedWorldState() );

		PendedWorldState& addedState = m_vPendedWorldStateList.back();
		addedState.itemcode = itemcode;
		addedState.code = code;
		addedState.level = level;
		addedState.time = time;
		addedState.cooltime = cooltime;

		return true;
	}

	return true;
}

const bool StructPlayer::CastWorldState( const StructState::StateCode code, const int level, const AR_TIME time, const AR_TIME cooltime, const int itemcode )
{
	// 없는 지속효과 걸려고 한 거면 실패
	const StateInfo* info = GameContent::GetStateInfo( code );

	if( info == NULL )
	{
		return false;
	}

	if( itemcode != 0 )
	{
		PrintfGlobalChatMessage( CHAT_NOTICE, "@NOTICE", "@2932\v#@avatar_name@#\v%s\v#@use_item_name@#\v@%d", GetName(), StructItem::GetItemBase( itemcode ).nNameId );
	}

	std::vector< StructPlayer * > pvPlayerList;
	_PlayerListComposer _plc( pvPlayerList );

	{
		ARCADIA_LOCK( ArcadiaServer::Instance().LockWorld() );
		
		DoEachPlayer( _plc );

		AR_TIME start_time = GetArTime();

		for( std::vector< StructPlayer * >::iterator it = pvPlayerList.begin(); it != pvPlayerList.end(); ++it )
		{
			StructPlayer* player = *it;

			player->AddState( code, GetHandle(), level, start_time, start_time + time, false, 0, GetName() );

			if( player->GetMainSummon() )
			{
				player->GetMainSummon()->AddState( code, GetHandle(), level, start_time, start_time + time, false, 0, GetName() );
			}
			
			if( player->GetSubSummon() )
			{
				player->GetSubSummon()->AddState( code, GetHandle(), level, start_time, start_time + time, false, 0, GetName() );
			}
		}
	}
	return true;
}

const bool StructPlayer::PendPartyState( const StructState::StateCode code, const int level, const AR_TIME time, const AR_TIME cooltime, const int itemcode )
{
	if(false == IsInParty())
	{
		return false;
	}

	const StateInfo* info = GameContent::GetStateInfo( code );

	if( info == NULL )
	{
		return false;
	}

	int remainTime = RefreshRemainTimeForWorldStateCast( code, cooltime );

	if( remainTime > 0 )
	{
		PrintfChatMessage( true, CHAT_ITEM, "@SYSTEM",  this, "@2931\v#@world_buff_cooltime@#\v%d:%d", remainTime / 60, remainTime % 60 );
		return false;
	}
	else
	{
		m_vPendedPartyStateList.push_back( PendedWorldState() );

		PendedWorldState& addedState = m_vPendedPartyStateList.back();
		addedState.itemcode = itemcode;
		addedState.code = code;
		addedState.level = level;
		addedState.time = time;
		addedState.cooltime = cooltime;

		return true;
	}

	return true;
}

const bool StructPlayer::CastPartyState( const StructState::StateCode code, const int level, const AR_TIME time, const AR_TIME cooltime, const int itemcode )
{
	if(false == IsInParty())
	{
		return false;
	}

	// 없는 지속효과 걸려고 한 거면 실패
	const StateInfo* info = GameContent::GetStateInfo( code );

	if( info == NULL )
	{
		return false;
	}

	if( itemcode != 0 )
	{
		char msg[256];
		sprintf_s(msg, sizeof(msg), "@2932\v#@avatar_name@#\v%s\v#@use_item_name@#\v@%d", GetName(), StructItem::GetItemBase( itemcode ).nNameId);
		SendPartyChatMessage(GetName(), GetPartyID(), msg, static_cast<unsigned int>(strlen(msg)));
	}

	std::vector< StructPlayer * > pvPlayerList;
	_PlayerListComposer _plc( pvPlayerList );

	{
		ARCADIA_LOCK( ArcadiaServer::Instance().LockWorld() );

		DoEachPartyPlayer( _plc );

		AR_TIME start_time = GetArTime();

		for( std::vector< StructPlayer * >::iterator it = pvPlayerList.begin(); it != pvPlayerList.end(); ++it )
		{
			StructPlayer* player = *it;

			player->AddState( code, GetHandle(), level, start_time, start_time + time, false, 0, GetName() );

			if( player->GetMainSummon() )
			{
				player->GetMainSummon()->AddState( code, GetHandle(), level, start_time, start_time + time, false, 0, GetName() );
			}

			if( player->GetSubSummon() )
			{
				player->GetSubSummon()->AddState( code, GetHandle(), level, start_time, start_time + time, false, 0, GetName() );
			}
		}
	}
	return true;
}

const bool StructPlayer::PendGuildState( const StructState::StateCode code, const int level, const AR_TIME time, const AR_TIME cooltime, const int itemcode )
{
	if(false == IsInGuild())
	{
		return false;
	}

	const StateInfo* info = GameContent::GetStateInfo( code );

	if( info == NULL )
	{
		return false;
	}

	int remainTime = RefreshRemainTimeForWorldStateCast( code, cooltime );

	if( remainTime > 0 )
	{
		PrintfChatMessage( true, CHAT_ITEM, "@SYSTEM",  this, "@2931\v#@world_buff_cooltime@#\v%d:%d", remainTime / 60, remainTime % 60 );
		return false;
	}
	else
	{
		m_vPendedGuildStateList.push_back( PendedWorldState() );

		PendedWorldState& addedState = m_vPendedGuildStateList.back();
		addedState.itemcode = itemcode;
		addedState.code = code;
		addedState.level = level;
		addedState.time = time;
		addedState.cooltime = cooltime;

		return true;
	}

	return true;
}

const bool StructPlayer::CastGuildState( const StructState::StateCode code, const int level, const AR_TIME time, const AR_TIME cooltime, const int itemcode )
{
	if(false == IsInGuild())
	{
		return false;
	}

	// 없는 지속효과 걸려고 한 거면 실패
	const StateInfo* info = GameContent::GetStateInfo( code );

	if( info == NULL )
	{
		return false;
	}

	if( itemcode != 0 )
	{
		char msg[256];
		sprintf_s(msg, sizeof(msg), "@2932\v#@avatar_name@#\v%s\v#@use_item_name@#\v@%d", GetName(), StructItem::GetItemBase( itemcode ).nNameId);
		SendGuildChatMessage(GetName(), GetPartyID(), msg, static_cast<unsigned int>(strlen(msg)));
	}

	std::vector< StructPlayer * > pvPlayerList;
	_PlayerListComposer _plc( pvPlayerList );

	{
		ARCADIA_LOCK( ArcadiaServer::Instance().LockWorld() );

		DoEachGuildPlayer( _plc );

		AR_TIME start_time = GetArTime();

		for( std::vector< StructPlayer * >::iterator it = pvPlayerList.begin(); it != pvPlayerList.end(); ++it )
		{
			StructPlayer* player = *it;

			player->AddState( code, GetHandle(), level, start_time, start_time + time, false, 0, GetName() );

			if( player->GetMainSummon() )
			{
				player->GetMainSummon()->AddState( code, GetHandle(), level, start_time, start_time + time, false, 0, GetName() );
			}

			if( player->GetSubSummon() )
			{
				player->GetSubSummon()->AddState( code, GetHandle(), level, start_time, start_time + time, false, 0, GetName() );
			}
		}
	}
	return true;
}

const bool StructPlayer::AddDungeonState()
{
	AR_TIME tStartTime = GetArTime();

	AddState( StructState::STATE_FOR_DUNGEON_OWNER_1, GetHandle(), 1, tStartTime, -1, true, 0, "", true );
	AddState( StructState::STATE_FOR_DUNGEON_OWNER_2, GetHandle(), 1, tStartTime, -1, true, 0, "", true );

	return true;
}

const bool StructPlayer::RemoveDungeonState()
{
	RemoveState( StructState::STATE_FOR_DUNGEON_OWNER_1, GameRule::MAX_STATE_LEVEL, true );
	RemoveState( StructState::STATE_FOR_DUNGEON_OWNER_2, GameRule::MAX_STATE_LEVEL, true );

	return true;
}

const bool StructPlayer::AddEventState( const StructState::StateCode eCode, const int nLevel, const int nStateValue, const char * szStateValue )
{
	// 없는 지속효과 걸려고 한 거면 실패
	if( !GameContent::GetStateInfo( eCode ) )
		return false;

	EVENT_STATE es( eCode, nLevel, nStateValue, szStateValue );

	std::vector< StructPlayer * > pvPlayerList;
	_PlayerListComposer _plc( pvPlayerList );

	{
		THREAD_SYNCHRONIZE( s_csEventState );

		bool bAlreadyExist = false;

		for( std::vector< EVENT_STATE >::iterator it = s_vEventState.begin() ; it != s_vEventState.end() ; ++it )
		{
			if( (*it).eCode != eCode )
				continue;

			// 완전히 같은 이벤트 지속효과를 지정할 경우 성공했다고 반환하지만 아무것도 하지 않음
			if( (*it) == es )
				return true;

			// 현재 지정되어 있는 같은 코드의 이벤트 지속효과보다 레벨이 낮으면 취소
			if( (*it).nLevel > nLevel )
				return false;

			(*it).nStateValue = nStateValue;
			(*it).strStateValue = szStateValue;

			bAlreadyExist = true;
			break;
		}

		if( !bAlreadyExist )
		{
			s_vEventState.push_back( es );
		}

		{
			ARCADIA_LOCK( ArcadiaServer::Instance().LockWorld() );

			DoEachPlayer( _plc );

			for( std::vector< StructPlayer * >::iterator it = pvPlayerList.begin(); it != pvPlayerList.end(); ++it )
			{
				(*it)->AddState( es.eCode, (*it)->GetHandle(), es.nLevel, GetArTime(), -1, true, es.nStateValue, es.strStateValue.c_str(), true );
			}
		}
	}

	return true;
}

const bool StructPlayer::RemoveEventState( const StructState::StateCode eCode )
{
	EVENT_STATE es;

	std::vector< StructPlayer * > pvPlayerList;
	_PlayerListComposer _plc( pvPlayerList );

	{
		THREAD_SYNCHRONIZE( s_csEventState );

		bool bExist = false;

		for( std::vector< EVENT_STATE >::iterator it = s_vEventState.begin() ; it != s_vEventState.end() ; ++it )
		{
			if( (*it).eCode != eCode )
				continue;

			es = (*it);
			s_vEventState.erase( it );

			bExist = true;
			break;
		}

		if( !bExist )
			return false;

		{
			ARCADIA_LOCK( ArcadiaServer::Instance().LockWorld() );

			DoEachPlayer( _plc );

			for( std::vector< StructPlayer * >::iterator it = pvPlayerList.begin(); it != pvPlayerList.end(); ++it )
			{
				(*it)->RemoveState( es.eCode, GameRule::MAX_STATE_LEVEL, true );
			}
		}
	}

	return true;
}

void StructPlayer::ClearEventState()
{
	std::vector< StructPlayer * > pvPlayerList;
	_PlayerListComposer _plc( pvPlayerList );

	{
		THREAD_SYNCHRONIZE( s_csEventState );

		if( s_vEventState.empty() )
			return;

		{
			ARCADIA_LOCK( ArcadiaServer::Instance().LockWorld() );

			DoEachPlayer( _plc );

			for( std::vector< EVENT_STATE >::const_iterator it = s_vEventState.begin() ; it != s_vEventState.end() ; ++it )
			{
				for( std::vector< StructPlayer * >::iterator pit = pvPlayerList.begin(); pit != pvPlayerList.end(); ++pit )
				{
					(*pit)->RemoveState( (*it).eCode, GameRule::MAX_STATE_LEVEL, true );
				}
			}
		}

		s_vEventState.clear();
	}
}

void StructPlayer::GetEventStateList( std::vector< struct EVENT_STATE > & vList )
{
	THREAD_SYNCHRONIZE( s_csEventState );

	vList = s_vEventState;
}

void StructPlayer::ProcEventState()
{
	THREAD_SYNCHRONIZE( s_csEventState );

	if( s_vEventState.empty() )
		return;

	AR_TIME tCurrent = GetArTime();

	{
		ARCADIA_LOCK( ArcadiaServer::Instance().LockObjectWithVisibleRange( this ) );

		for( std::vector< EVENT_STATE >::const_iterator it = s_vEventState.begin() ; it != s_vEventState.end() ; ++it )
		{
			AddState( (*it).eCode, GetHandle(), (*it).nLevel, tCurrent, -1, true, (*it).nStateValue, (*it).strStateValue.c_str(), true );
		}
	}
}

void StructPlayer::SendCharacterInfo()
{
	// 스탯 재계산
	CalculateStat();

	quadTreeItem.Set( *this );
	quadTreeItem.AddMe();

	// 로그인 세팅
	Login();

	// 크리쳐 정보 전송
	{
		std::vector< StructSummon* >::iterator it;
		for( it = m_vSummonList.begin(); it != m_vSummonList.end(); ++it ) SendAddSummonMessage( this, *it );
	}

	// 펫 정보 전송
	{
		std::vector< StructPet * >::iterator it;
		for( it = m_vPetList.begin() ; it != m_vPetList.end() ; ++it )
			SendAddPetMessage( this, (*it) );
	}

	// 인벤정보 날려주자
	SendItemList( this, false );

	// 바인딩된 크리쳐 
	SendCreatureEquipMessage( this, false );

	// 크리쳐 스킬정보 전송
	{
		std::vector< StructSummon* >::iterator it;
		for( it = m_vSummonList.begin(); it != m_vSummonList.end(); ++it )
		{
			SendSkillMessage( this, *it );
			SendAddedSkillMessage( this, *it );
		}
	}

	// 크리쳐 장비정보 전송
	{
		std::vector< StructSummon* >::iterator it;
		for( it = m_vSummonList.begin(); it != m_vSummonList.end(); ++it ) SendTotalItemWearInfoMessage( this, *it );
	}

	// 장비정보 알려주자
	SendWearMessage( this );	

	// 돈, 라크 정보 전송
	SendGoldChaosUpdateMsg( this );

	// 특성포인트 전송
	SendPropertyMessage( this, GetHandle(), "tp", GetTalentPoint() );

	// 혼돈 양 전송
	SendPropertyMessage( this, GetHandle(), "chaos", GetChaos() );

	// 레벨/경험치
	SendLevelMsg( this, this );
	SendExpMsg( this, this );

	// 직업정보/직업레벨 전송
	SendJobInfo( this, this );	

	// 스킬정보 전송
	SendSkillMessage( this, this );
	SendAddedSkillMessage( this, this );

	// 은신 감지 범위 전송
	SendDetectRangeMessage( this, this );

	// 벨트 슬롯 정보
	SendBeltSlotInfo( this );

	// game time
	SendGameTime( this );

	// 헌터홀릭 포인트
	SendPropertyMessage( this, GetHandle(), "huntaholicpoint", GetHuntaholicPoint() );

	// 남은 헌터홀릭 입장 횟수
	SendPropertyMessage( this, GetHandle(), "huntaholic_ent", GetHuntaholicEnterableCount() );

	// 배틀 아레나 관련 내용
	SendPropertyMessage( this, GetHandle(), "ap", GetBattleArenaPoint() );
	BattleArenaManager::SendBattleArenaPenaltyMessage( this );

	// 별칭
	SendPropertyMessage( this, GetHandle(), "alias", GetAlias( true ) );

	// 에테리얼 스톤 내구도
	SendPropertyMessage( this, GetHandle(), "ethereal_stone", GetEtherealStoneDurability() );

	SendPropertyMessage( this, GetHandle(), "dk_count", GetDKC() );
	SendPropertyMessage( this, GetHandle(), "pk_count", GetPKC() );
	SendPropertyMessage( this, GetHandle(), "immoral", GetImmoralPoint().get() );
	SendPropertyMessage( this, GetHandle(), "permission", GetPermission() );


	// 바인딩된 스킬카드 정리
	{
		size_t idx;
		for( idx = 0; idx < m_Inventory.GetCount(); ++idx )
		{
			StructItem *pItem = m_Inventory.Get( idx );
			if( pItem->IsSkillCard() )
			{
				// 이미 카드에 설정된 SID와 PlayerUID가 동일함을 확인하였기에 다시 저장할 필요가 없다.
				if( pItem->GetBindedPlayerSID() == GetPlayerUID() ) BindSkillCard( pItem, true );
				else if( pItem->GetBindedSummonSID() ) 
				{
					// 현재는 소환수에 스킬 카드를 바인딩할 수 없으며 해당 시스템을 사용하기 위해서는 구현이 추가로 필요함.
					// 스킬 카드가 바인딩 되어있는 소환수이더라도 삭제(버리기, 거래, 판매 등)가 가능하므로
					// 아이템에 바인딩된 SummonSID로 소환수를 검색하였을 때 pSummon이 NULL일 가능성이 존재
					// 호출이 안 될 뿐더러, 사실상 사용도 할 수 없는 코드
					assert( 0 );
					StructSummon *pSummon = GetSummon(pItem->GetBindedSummonSID());
					pSummon->BindSkillCard( pItem, true );
				}
			}
		}
	}

	ARCADIA_LOCK( ArcadiaServer::Instance().LockObjectWithVisibleRange( this ) );

	if( !IsInWorld() )
	{
		ChannelManager::EnterPlayerToLayer( GetLayer() );
		// 월드에 플레이어 추가

		ArcadiaServer::Instance().AddObject( this );

		// enter 이펙트 끄자
		SetFirstEnter( false );	
	}

	if( GetSubSummon() )
	{

		GetSubSummon()->SetInvincible( true );
		// 월드에 추가
		ArPosition valid_pos;
		if( GameContent::GetValidRandomPos( GetPos(), valid_pos, StructSummon::SUMMON_SPAWN_LENGTH ) == false )
		{
			valid_pos = GetPos();
		}

		GetSubSummon()->SetCurrentXY( valid_pos.x, valid_pos.y );
		GetSubSummon()->SetCurrentLayer( GetLayer() );
		AddSummonToWorld( GetSubSummon() );

		AR_TIME t = GetArTime();
		AR_TIME nNextUnSummonTime = ( m_nNextUnSummonTime > t ) ? ( m_nNextUnSummonTime - t ) : 0;

		TS_SC_UNSUMMON_NOTICE msg;

		msg.summon_handle = m_pSubSummon->GetHandle();
		msg.unsummon_duration = nNextUnSummonTime;

		PendMessage( this, &msg );
	}

	if( GetMainSummon() )
	{
		GetMainSummon()->SetInvincible( true );
		// 월드에 추가
		ArPosition valid_pos;
		if( GameContent::GetValidRandomPos( GetPos(), valid_pos, StructSummon::SUMMON_SPAWN_LENGTH ) == false )
		{
			valid_pos = GetPos();
		}

		GetMainSummon()->SetCurrentXY( valid_pos.x, valid_pos.y );
		GetMainSummon()->SetCurrentLayer( GetLayer() );
		AddSummonToWorld( GetMainSummon() );
	}

	if( GetSummonedPet() )
	{
		GetSummonedPet()->SetInvincible( true );
		//월드에 추가
		ArPosition valid_pos;
		if( GameContent::GetValidRandomPos( GetPos(), valid_pos, StructPet::PET_SPAWN_MIN_LENGTH ) == false )
		{
			valid_pos = GetPos();
		}

		GetSummonedPet()->SetCurrentXY( valid_pos.x, valid_pos.y );
		GetSummonedPet()->SetCurrentLayer( GetLayer() );
		AddPetToWorld( GetSummonedPet() );
	}

	// 상태이상 정보 전송
	{
		std::vector< StructState >::iterator it;

		for( it = m_vStateList.begin(); it != m_vStateList.end(); ++it )
		{
			onUpdateState( (*it), false );
		}
	}

	ArPosition pos = GetPos();

	SendPropertyMessage( this, GetHandle(), "stamina", GetStamina() );
	SendPropertyMessage( this, GetHandle(), "max_stamina", m_nMaxStamina );

	SendPropertyMessage( this, GetHandle(), "channel", ChannelManager::GetChannelNum( layer ) );

	BroadcastStatusMessage( this );
}

int getJPForSummon( int jp, bool bIsActive )
{
	if( bIsActive )
	{
		if( jp <= 3 )		return 0;
		else if( jp <= 7	)	return 1;
		else if( jp <= 12	)	return 2;
		else if( jp <= 18	)	return 3;
		else if( jp <= 25	)	return 4;
		else if( jp <= 33	)	return 5;
		else if( jp <= 45	)	return 6;
		else if( jp <= 59	)	return 7;
		else if( jp <= 75	)	return 8;
		else if( jp <= 99	)	return 9;
		else if( jp <= 299	)	return jp * 0.1f;
		else					return jp * 0.1f;
	}
	else
	{
		if( jp <= 3 )		return 0;
		else if( jp <= 7	)	return 0;
		else if( jp <= 12	)	return 0;
		else if( jp <= 18	)	return 0;
		else if( jp <= 25	)	return 1;
		else if( jp <= 33	)	return 1;
		else if( jp <= 45	)	return 1;
		else if( jp <= 59	)	return 1;
		else if( jp <= 75	)	return 1;
		else if( jp <= 99	)	return 1;
		else if( jp <= 299	)	return 2;
		else					return 3;
	}
}
void StructPlayer::AddChaos( int nChaos )
{
	m_nChaos += nChaos;

	if( GetChaos() > GetMaxChaos() )
		m_nChaos = GetMaxChaos();

	if( m_nChaos < 0 )
		m_nChaos = 0;

	m_QuestManager.UpdateQuestStatusByParameter( QuestBase::QUEST_PARAMETER_CHAOS, GetChaos() );

	SendPropertyMessage( this, GetHandle(), "chaos", m_nChaos );
}


void StructPlayer::applyPenaltyByImmoralPoint( Experience& result )
{
	if( GetImmoralPoint() > GameRule::CRIME_LIMIT )
	{
		result.exp -= result.exp * 0.9f;
	}
	else if( GetImmoralPoint() > GameRule::MORAL_LIMIT )
	{
		result.exp -= result.exp * int( GetImmoralPoint() / 100.0f ) * 0.09f;
	}
}

void StructPlayer::applyLimitBySummonLevel( Experience& result )
{
	if( ( GetMainSummon() && GetMainSummon()->GetLevel() > GetLevel() + GameRule::SummonExpPenaltyLevel) || ( GetSubSummon() && GetSubSummon()->GetLevel() > GetLevel() + GameRule::SummonExpPenaltyLevel) )
	{
		result.exp = std::min( result.exp, static_cast< __int64 >( GameRule::GetPlayerEXPLimit( GetLevel() ) ) );
	}
}

void StructPlayer::applyPenaltyByGameTimeLimit( Experience& result )
{
	if( IsGameTimeLimited() )
	{
		c_fixed10 fGameTimeLimitPenalty = GameRule::GetGameTimeLimitPenalty( GetContinuousPlayTime() );

		result.exp = fGameTimeLimitPenalty * result.exp;
		result.jp = fGameTimeLimitPenalty * result.jp;
	}
}

const StructCreature::Experience StructPlayer::getPCBangBonus( const Experience& input )
{
	Experience bonus;

	switch( GetPCBangMode() )
	{
	case GameRule::PCBANG_ALLY_BONUS:
		bonus.exp = GameRule::fAllyPCBangBonusRate * input.exp;
		bonus.jp = GameRule::fAllyPCBangBonusRate * input.jp;

		if( bonus.exp || bonus.jp )
		{
			setBonusMsg( BONUS_PCBANG, GameRule::fAllyPCBangBonusRate * 100, bonus.exp, bonus.jp );
		}
		break;
	case GameRule::PCBANG_PREMIUM_BONUS:
		bonus.exp = GameRule::fPremiumPCBangBonusRate * input.exp;
		bonus.jp = GameRule::fPremiumPCBangBonusRate * input.jp;

		if( bonus.exp || bonus.jp )
		{
			setBonusMsg( BONUS_PREMIUM_PCBANG, GameRule::fPremiumPCBangBonusRate * 100, bonus.exp, bonus.jp );
		}
		break;
	default:
		break;
	}

	return bonus;
}

const StructCreature::Experience StructPlayer::getStaminaBonus( const Experience& input, const __int64 gainExp )
{
	Experience bonus;

	// 프리미엄 PC방에서 스테미너 보너스 금지 설정이 되어 있다면 프리미엄 PC방에서는 스테 효과 받으면 안됨
	if( GetPCBangMode() != GameRule::PCBANG_PREMIUM_BONUS || GameRule::bApplyStaminaBonusInPremiumPCBang )
	{
		float staminaRatio = GameRule::GetStaminaRatio( GetLevel() );

		if( GetStamina() >= gainExp/staminaRatio || m_bStaminaSave )
		{
			bonus.exp = GameRule::fStaminaBonusRate * input.exp;
			bonus.jp = GameRule::fStaminaBonusRate * input.jp;
		}

		if( !m_bStaminaSave )
			AddStamina( 0 - gainExp/staminaRatio );
	}

	if( bonus.exp || bonus.jp )
	{
		setBonusMsg( BONUS_STAMINA, GameRule::fStaminaBonusRate * 100, bonus.exp, bonus.jp );
	}

	return bonus;
}

const StructCreature::Experience StructPlayer::getSuperSaveBonus( const Experience& input )
{
	Experience bonus;

	// 성장의 물약은 PC방과 상관없이 레벨만 만족한다면 무조건 혜택을 받으며, 기존의 스테미너 시스템과는 완벽히 분리되어 있다.
	if( m_bSuperSave )
	{
		bonus.exp = GameRule::fSuperSaveBonusRate * input.exp;
		bonus.jp = GameRule::fSuperSaveBonusRate * input.jp;

		if( bonus.exp || bonus.jp )
		{
			setBonusMsg( BONUS_SUPER_SAVE, GameRule::fSuperSaveBonusRate * 100, bonus.exp, bonus.jp );
		}
	}

	return bonus;
}

const StructCreature::Experience StructPlayer::getInGameBonus( const Experience& input )
{
	Experience bonus;

	bonus.exp = m_fEXPMod * input.exp;
	bonus.jp = m_fEXPMod * input.jp;

	if( bonus.exp || bonus.jp )
	{
		setBonusMsg( BONUS_IN_GAME, m_fEXPMod * 100, bonus.exp, bonus.jp );
	}

	return bonus;
}

void StructPlayer::distributeExpToSummons( const Experience& input, const __int64 summonStaminaSaveBonus )
{
	AR_TIME t = GetArTime();
	ArPosition pos = GetCurrentPosition( t );

	std::vector< StructSummon* >::iterator it;
	std::vector< StructSummon* > vActiveSummonList;
	std::vector< StructSummon* > vDeActiveSummonList;

	for( int i = 0; i < 6; ++i )
	{
		StructSummon *pSummon = GetSummonAt( i );
		if( !pSummon ) continue;
		if( pSummon->IsDead() ) continue;	// 죽었으면 KIN
		if( pSummon->GetRidingInfo() == SummonBase::RIDING_LENT ) continue;	// 라이딩 전용 대여 크리쳐면 즐

		if( pSummon->IsInWorld() )
		{
			// 시야 거리 밖이면 경험치를 아예 못 먹도록 수정
			if( pos.GetDistance( pSummon->GetCurrentPosition( t ) ) <=  GameRule::VISIBLE_RANGE )
			{
				vActiveSummonList.push_back( pSummon );
			}
		}
		else
		{
			vDeActiveSummonList.push_back( pSummon );
		}
	}

	// 활동중인 소환수들한테 떼어주자
	if( !vActiveSummonList.empty() )
	{
		__int64 nActivateSummonExp = GameRule::GetIntValueByRandom< __int64 >( input.exp * m_fActiveSummonExpAmp );
		bool bBonusSupplied = false;

		for( it = vActiveSummonList.begin(); it != vActiveSummonList.end(); ++it )
		{
			// 경험치 제한 체크
			if( (*it)->IsEXPLimitReached() )
				continue;

			// 소환수도 몬스터 사냥으로 인해 습득한 경험치를 100%로 먹는다.
			// 단, 캐릭터보다 레벨이 높거나 같으면 사냥 경험치 습득하지 못 함
			__int64 nGainExp = ( (*it)->GetLevel() < GetLevel() ) ? nActivateSummonExp : 0;

			// 소환수 스테미너 세이버 효과 적용
			if( m_eSummonStaminaSaveType == SUMMON_STAMINA_SAVE_WITHOUT_PENALTY || ( m_eSummonStaminaSaveType == SUMMON_STAMINA_SAVE_WITH_PENALTY && (*it)->GetLevel() < GetLevel() ) )
			{
				bBonusSupplied = true;
				nGainExp += summonStaminaSaveBonus;
			}

			if( !nGainExp )
				continue;

			(*it)->AddExp( nGainExp, 0 );
		}

		if( bBonusSupplied )
			setBonusMsg( BONUS_SUMMON_STAMINA, GameRule::fSummonStatminaSaveBonusRate * 100, summonStaminaSaveBonus, 0 );
	}

	// 대기중인 소환수들한테 떼어주자
	if( !vDeActiveSummonList.empty() )
	{
		// 대기 중인 소환수는 몬스터 사냥으로 인해 경험치를 습득하지 못한다.
		// 단, 크리처 브리딩에 의해 소환수 배분 경험치를 증가시킨 만큼은 획득함.
		__int64 nDeActiveSummonEXP = GameRule::GetIntValueByRandom< __int64 >( input.exp * m_fDeactiveSummonExpAmp );

		for( it = vDeActiveSummonList.begin(); it != vDeActiveSummonList.end(); ++it )
		{
			(*it)->AddExp( ( (*it)->GetLevel() >= GetLevel() || (*it)->IsEXPLimitReached() ) ? 0 : nDeActiveSummonEXP, 0 );
		}
	}
}

void StructPlayer::applyGameRuleLimit( Experience& result )
{
	if( GameRule::nMaxLevel > 0 )
	{
		__int64 nMaxEXP = GameContent::GetNeedExp( GameRule::nMaxLevel ) - 1; // Epic 9.1 - MaxLevel 99.99%까지 경험치 얻을 수 있게 변경
		if( GetEXP() + result.exp > nMaxEXP )
		{
			if( GetEXP() >= nMaxEXP )
			{
				result.exp = 0;
			}
			else
			{
				result.exp = nMaxEXP - GetEXP();
			}
		}
	}

	if( result.jp > 0 )
	{
		if( m_nJobPoint >= GameRule::MAX_OWNABLE_JP )
		{
			result.jp = 0;
			SendChatMessage( false, CHAT_EXP, "@SYSTEM", this, "@6759" );
		}
		else if( GameRule::MAX_OWNABLE_JP - m_nJobPoint < result.jp )
			result.jp = GameRule::MAX_OWNABLE_JP - m_nJobPoint;
	}

}


void StructPlayer::AddExp( __int64 exp, __int64 jp, bool bApplyStamina )
{
	Experience result( exp, jp );

	applyPenaltyByImmoralPoint( result );
	applyLimitBySummonLevel( result );

	__int64 gain_exp = result.exp;	// 스테 계산시 경치 증폭 전에 해당하는 수치로 감소시켜야 함.

	applyPenaltyByGameTimeLimit( result );

	if( result.exp == 0 && result.jp == 0 )
		return;

	// 소환수 스테미너 세이버(오곡 크래커) 효과로 적용할 경험치 양을 이 시점의 경험치 양 기준으로 계산(이 시점의 경험치 * 50%를 별도 증가 처리)
	__int64 nSummonStaminaSaveBonusEXP = ( m_eSummonStaminaSaveType != SUMMON_STAMINA_SAVE_NONE ) ? result.exp * GameRule::fSummonStatminaSaveBonusRate : 0;

	Experience bonus;

	// 중국 게임 중독 방지 시스템을 사용할 경우 건강 게임 시간에만 스테 및 PC방 보너스 적용 됨
	if( !IsGameTimeLimited() || GetContinuousPlayTime() < GameRule::nMaxHealthyGameTime )
	{
		result += getPCBangBonus( result );

		if( bApplyStamina )
		{
			bonus += getStaminaBonus( result, gain_exp );
			bonus += getSuperSaveBonus( result );
		}
	}

	bonus += getInGameBonus( result );

	result += bonus;

	distributeExpToSummons( result, nSummonStaminaSaveBonusEXP );
	applyGameRuleLimit( result );

	StructCreature::AddExp( result.exp, result.jp );
}

void StructPlayer::CloseStorage()
{
	m_nIsUsingStorage = CLOSE_STORAGE;
	return;
}

void StructPlayer::onAdd( struct StructInventory* pInventory, struct StructItem* pItem, bool bSkipUpdateItemToDB )
{
	int nPrevOwnerUID = pItem->GetOwnerUID();
	int nPrevAccountID = pItem->GetAccountID();

	if( pInventory == &m_Inventory )
	{
		pItem->SetOwnerInfo( GetHandle(), GetPlayerUID(), 0 );

		if( pItem->IsSummonCard() )
		{	
			StructSummon *pSummon = pItem->GetSummonStruct();
			if( pSummon )
			{
				AddSummon( pSummon, bSkipUpdateItemToDB );
				SendSkillMessage( this, pSummon );
			}
		}
		else if( pItem->IsPetCage() )
		{
			StructPet * pPet = pItem->GetPetStruct();
			if( pPet )
			{
				AddPet( pPet, bSkipUpdateItemToDB );
			}
		}

		if( pItem->GetWearType() == ItemBase::WEAR_RIDE_ITEM && IsLoginComplete() && m_anWear[ItemBase::WEAR_RIDE_ITEM] == NULL )
		{
			putonItem( ItemBase::WEAR_RIDE_ITEM, pItem );
		}

		if( pItem->IsCharm() || pItem->IsCollection())
		{
			m_vCharmList.push_back( pItem ); 

			if( IsLoginComplete() )
			{
				CalculateStat();

				if( IsRiding() )
				{
					StructSummon * pSummon = GetRideObject();

					if( pSummon )
					{
						pSummon->CalculateStat();
					}
				}
			}
		}

		if( pItem->GetElementalEffectType() && pItem->GetElementalEffectExpireTime() )
		{
			AddToElementalEffectedItemList( pItem );
		}

		// 강화 단계가 0인 녀석이 추가되어도 아이템 강화 관련 퀘스트에는 영향을 주지 않는다
		// 그다지 마음에 드는 방법은 아니나 가급적 불필요한 오버헤드를 피하기 위한 방책
		if( pItem->GetItemEnhance() > 0 )
		{
			m_QuestManager.UpdateQuestStatusByItemEnhance( &m_Inventory, pItem->GetItemCode() );
		}

		m_QuestManager.UpdateQuestStatusByItemCount( pItem->GetItemCode(), pItem->GetCount() );
		m_TitleManager.UpdateTitleConditionByItemGet( pItem->GetItemCode(), pItem->GetCount() );
		if( pItem->IsSummonCard() && pItem->GetSummonCode() )
			m_TitleManager.UpdateTitleConditionBySummonCardGet( pItem->GetSummonCode(), GameContent::GetSummonInfo( pItem->GetSummonCode() )->rate, pItem->GetInstanceFlag().IsOn( ItemInstance::ITEM_FLAG_SUMMON ), true );

		// 아이템을 습득 했을때 각성이 되어 있는 아이템이라면 소유자를 변경해야 한다.
		if( !bSkipUpdateItemToDB && pItem->IsAwaken() )
		{
			DBQuery( new DB_UpdateRandomOptionOwnerInfo( this, pItem->GetAwakenSID(), GetPlayerUID(), 0 ) );
		}

		// 아이템을 습득 했을때 랜덤화 되어 있는 아이템이라면 소유자를 변경해야 한다.
		if( !bSkipUpdateItemToDB && pItem->IsIdentified() )
		{
			DBQuery( new DB_UpdateRandomOptionOwnerInfo( this, pItem->GetIdentifiedSID(), GetPlayerUID(), 0 ) );
		}
	}
	else
	{
		pItem->SetOwnerInfo( GetHandle(), 0, GetAccountID() );

		if( pItem->IsSummonCard() )
		{
			StructSummon *pSummon = pItem->GetSummonStruct();
			if( pSummon )
			{
				AddSummonToStorage( pSummon, bSkipUpdateItemToDB );
			}
		}
		else if( pItem->IsPetCage() )
		{
			StructPet * pPet = pItem->GetPetStruct();
			if( pPet )
			{
				AddPetToStorage( pPet, bSkipUpdateItemToDB );
			}
		}

		// 각성, 랜덤화된 아이템의 경우 창고에 넣을 때 소유자 정보 제거 / 계정 정보로 owner info 업데이트 해준다
		if( !bSkipUpdateItemToDB && pItem->IsAwaken() )
		{
			DBQuery( new DB_UpdateRandomOptionOwnerInfo( this, pItem->GetAwakenSID(), 0, GetAccountID() ) );
		}
		if( !bSkipUpdateItemToDB && pItem->IsIdentified() )
		{
			DBQuery( new DB_UpdateRandomOptionOwnerInfo( this, pItem->GetIdentifiedSID(), 0, GetAccountID() ) );
		}
	}

	// UID 가 없던 것이었다면 할당후 DB 에 insert
	if( !pItem->GetItemUID() )
	{
		// UID 발급
		pItem->SetItemUID( allocItemUID() );

		// DB 에 insert
		if( !bSkipUpdateItemToDB )
		{
			pItem->DBQuery( new DB_InsertItem( pItem ) );
		}
	}
	else
	{
		// owner 바뀐것 DB에 기록
		if( !bSkipUpdateItemToDB && ( nPrevOwnerUID != pItem->GetOwnerUID() || nPrevAccountID != pItem->GetAccountID() ) && IsLoginComplete() )
		{
			pItem->DBQuery( new DB_UpdateItemOwner( pItem ) );
		}
	}

	if( IsLogin() )	SendItemMessage( this, pItem );

	UpdateWeightWithInventory();
}

void StructPlayer::onRemove( struct StructInventory* pInventory, struct StructItem* pItem, bool bSkipUpdateItemToDB )
{
	if( IsLogin() )	
	{
		// 주인 없는 아이템으로 DB에 저장.
		pItem->SetOwnerInfo( NULL, 0, 0 );
		if( !bSkipUpdateItemToDB && pItem->GetItemUID() ) 
		{
			pItem->DBQuery( new DB_UpdateItemOwner( pItem ) );
		}

		if( pInventory == &m_Inventory )
		{
			// 소환수 아템일경우
			if( pItem->IsSummonCard() )
			{	
				StructSummon *pSummon = pItem->GetSummonStruct();
				if( pSummon )
				{
					RemoveSummon( pSummon, bSkipUpdateItemToDB );
				}
			}
			// 펫 우리일 경우
			else if( pItem->IsPetCage() )
			{
				StructPet *pPet = pItem->GetPetStruct();
				if( pPet )
				{
					RemovePet( pPet, bSkipUpdateItemToDB );
				}
			}

			if( pItem->IsCharm() || pItem->IsCollection())
			{
				std::vector< StructItem * >::iterator it;

				for( it = m_vCharmList.begin(); it != m_vCharmList.end(); ++it )
				{
					if( (*it) == pItem )
					{
						m_vCharmList.erase( it );
						break;
					}
				}

				if( IsLoginComplete() )
				{
					CalculateStat();

					if( IsRiding() )
					{
						StructSummon * pSummon = GetRideObject();

						if( pSummon )
						{
							pSummon->CalculateStat();
						}
					}
				}
			}

			if( pItem->GetElementalEffectType() && pItem->GetElementalEffectExpireTime() )
			{
				RemoveFromElementalEffectedItemList( pItem );
			}

			// 바인드되어 있는 스킬카드라면..
			if( pItem->IsSkillCard() && pItem->GetBindedCreatureHandle() )
			{
				StructCreature::iterator it = StructCreature::get( pItem->GetBindedCreatureHandle() );
				(*it)->UnBindSkillCard( pItem );
			}

			// 강화 단계가 0인 녀석이 빠져도 아이템 강화 관련 퀘스트에는 영향을 주지 않는다
			// 그다지 마음에 드는 방법은 아니나 가급적 불필요한 오버헤드를 피하기 위한 방책
			if( pItem->GetItemEnhance() > 0 )
			{
				m_QuestManager.UpdateQuestStatusByItemEnhance( &m_Inventory, pItem->GetItemCode() );
			}

			m_QuestManager.UpdateQuestStatusByItemCount( pItem->GetItemCode(), 0 );
			m_TitleManager.UpdateTitleConditionByItemGet( pItem->GetItemCode(), 0 );
			if( pItem->IsSummonCard() && pItem->GetSummonCode() )
				m_TitleManager.UpdateTitleConditionBySummonCardGet( pItem->GetSummonCode(), GameContent::GetSummonInfo( pItem->GetSummonCode() )->rate, pItem->GetInstanceFlag().IsOn( ItemInstance::ITEM_FLAG_SUMMON ), false );
		}
		else // 창고
		{
			if( pItem->IsSummonCard() )
			{
				StructSummon *pSummon = pItem->GetSummonStruct();
				if( pSummon )
				{
					RemoveSummonFromStorage( pSummon, bSkipUpdateItemToDB );
				}
			}
			else if( pItem->IsPetCage() )
			{
				StructPet * pPet = pItem->GetPetStruct();
				if( pPet )
				{
					RemovePetFromStorage( pPet, bSkipUpdateItemToDB );
				}
			}
		}

		SendItemDestroyMessage( this, pItem );
	}

	UpdateWeightWithInventory();
}

void StructPlayer::onChangeCount( struct StructInventory* pInventory, struct StructItem* pItem, bool bSkipUpdateItemToDB )
{
	if( IsLogin() )
	{
		if( pInventory == &m_Inventory )
		{
			m_QuestManager.UpdateQuestStatusByItemCount( pItem->GetItemCode(), pItem->GetCount() );
			m_TitleManager.UpdateTitleConditionByItemGet( pItem->GetItemCode(), pItem->GetCount() );
		}

		SendItemCountMessage( this, pItem );
	}

	// 창고에서 빼낸경우 갯수를 즉시 업데이트 한다.
	if( !bSkipUpdateItemToDB && pItem->IsInStorage() )
	{
		pItem->DBQuery( new DB_UpdateItem( pItem ) );
	}

	UpdateWeightWithInventory();
}

// 퀘스트를 받았을때 초기 status 세팅
void StructPlayer::updateQuestStatus( StructQuest* pQuest )
{
	// { 기존 아이템 갯수 설정
	if( pQuest->GetQuestType() == QuestBase::QUEST_COLLECT ||
		pQuest->GetQuestType() == QuestBase::QUEST_HUNT_ITEM ||
		pQuest->GetQuestType() == QuestBase::QUEST_HUNT_ITEM_FROM_ANY_MONSTERS ||
		pQuest->GetQuestType() == QuestBase::QUEST_HUNT_ITEM_DROP_PENALTY )
	{
		int nMaxItemCollectTypeCount = ( pQuest->GetQuestType() == QuestBase::QUEST_COLLECT ) ? QuestInstance::MAX_STATUS : 3;

		for( int i = 0 ; i < nMaxItemCollectTypeCount ; ++i )
		{
			int nItemCode = pQuest->GetValue( i*2 );
			if( !nItemCode ) continue;

			StructItem *pItem = FindItem( nItemCode );
			if( !pItem ) continue;

			m_QuestManager.UpdateQuestStatusByItemCount( nItemCode, pItem->GetCount() );
		}
	}
	// }

	// { 기존 스킬 레벨 설정
	if( pQuest->GetQuestType() == QuestBase::QUEST_LEARN_SKILL )
	{
		m_QuestManager.UpdateQuestStatusBySkillLevel( pQuest->GetValue( 0 ), GetBaseSkillLevel( pQuest->GetValue( 0 ) ) );
		m_QuestManager.UpdateQuestStatusBySkillLevel( pQuest->GetValue( 2 ), GetBaseSkillLevel( pQuest->GetValue( 2 ) ) );
		m_QuestManager.UpdateQuestStatusBySkillLevel( pQuest->GetValue( 4 ), GetBaseSkillLevel( pQuest->GetValue( 4 ) ) );
	}
	// }

	// { 잡 레벨 설정
	if( pQuest->GetQuestType() == QuestBase::QUEST_JOB_LEVEL )
	{
		m_QuestManager.UpdateQuestStatusByJobLevel( GetJobDepth(), GetJobLevel() );
	}
	// }

	if( pQuest->GetQuestType() == QuestBase::QUEST_PARAMETER )
	{
		// 일단은 chaos 하나
		m_QuestManager.UpdateQuestStatusByParameter( QuestBase::QUEST_PARAMETER_CHAOS, GetChaos() );
	}

	// 아템 업글 퀘스트
	if( pQuest->GetQuestType() == QuestBase::QUEST_UPGRADE_ITEM )
	{
		UpdateQuestStatusByItemUpgrade();
	}

	if( pQuest->GetQuestType() == QuestBase::QUEST_ENHANCE_ITEM )
	{
		m_QuestManager.UpdateQuestStatusByItemEnhance( &m_Inventory );
	}
}

// 퀘스트를 받았을때 초기 status 세팅
void StructPlayer::onStartQuest( StructQuest* pQuest )
{
	updateQuestStatus( pQuest );

	SendNPCStatusInVisibleRange( this );
}

void StructPlayer::onEndQuest( StructQuest* pQuest )
{
	m_QuestManager.PopFromActiveQuest( pQuest );

	SendNPCStatusInVisibleRange( this );
}

void StructPlayer::onDropQuest( StructQuest* pQuest )
{
	// 퀘스트 포기시 라크 수집의 경우 라크 제거
	if( pQuest->GetQuestType() == QuestBase::QUEST_PARAMETER )
	{
		for( int i = 0 ; i < 4 ; ++i )
		{
			if( pQuest->GetValue( i * 3 ) != QuestBase::QUEST_PARAMETER_CHAOS )
				continue;

			if( !GetChaos() )
				continue;

			if( pQuest->GetValue( i * 3 + 1 ) == 1 )
			{
				AddChaos( -1 * pQuest->GetValue( i * 3 + 2 ) );
			}
		}
	}

	// 퀘스트를 제거하기 전에 스크립트 호출
	if( !pQuest->GetQuestBase().strDropScript.empty() )
	{
		char buf[256];
		s_sprintf( buf, _countof( buf ), "iocp.%d.script", GetCurrentThreadId() );
		ENV().Set( buf, pQuest->GetQuestBase().strDropScript.c_str() );

		ThreadPlayerHelper TPHelper( this );
		LUA()->RunString( pQuest->GetQuestBase().strDropScript.c_str() );
	}

	// 수행 중인 퀘스트 목록에서 퀘스트 제거
	m_QuestManager.PopFromActiveQuest( pQuest );

	// NPC 머리 위에 표시 갱신 클라 방송
	SendNPCStatusInVisibleRange( this );
}

bool StructPlayer::StartQuest( QuestBase::QuestCode code, int nStartQuestID, bool bForce )
{
	ARCADIA_LOCK( ArcadiaServer::Instance().LockObject( this ) );

	const QuestBase & rQuestBase = StructQuest::GetQuestBase( code );

	// 게임 중독 방지 시스템 적용(피로 게임 시간에는 퀘스트를 받을 수 없음)
	if( IsGameTimeLimited() )
	{
		if( GetContinuousPlayTime() >= GameRule::nMaxTiredGameTime )
		{
			SendQuestMessage( CHAT_QUEST_SYSTEM, this, "START|FAIL|GAME_TIME_LIMIT|%d", rQuestBase.nQuestTextId );
			return false;
		}
		else if( GetContinuousPlayTime() >= GameRule::nMaxHealthyGameTime )
		{
			// 경고 메시지는 뜨지만 퀘스트 수락은 가능하도록 진행
			SendQuestMessage( CHAT_QUEST_SYSTEM, this, "START|WARNING|GAME_TIME_LIMIT|%d", rQuestBase.nQuestTextId );
		}
	}

	// 퀘스트 보유 최대 개수 제한이 걸린 상태에서는 시작 대기 중이었던 퀘스트이거나 오토 퀘스트만 시작할 수 있음.
	if( m_QuestManager.GetActiveQuestCount() >= StructQuestManager::QUEST_MAX && !m_QuestManager.IsPendingQuest( code ) )
	{
		SendQuestMessage( CHAT_QUEST_SYSTEM, this, "START|FAIL|QUEST_NUMBER_EXCEED|%d", rQuestBase.nQuestTextId );
		return false;
	}

	// { 수행 가능한지 검사
	if( !bForce && !IsStartableQuest( code ) )
	{
		SendQuestMessage( CHAT_QUEST_SYSTEM, this, "START|FAIL|NOT_STARTABLE|%d", rQuestBase.nQuestTextId );
		return false;
	}
	// }

	bool bHasRandomQuest = false;

	if( StructQuest::IsRandomQuest( code ) )
	{
		if( m_QuestManager.HasRandomQuestInfo( code ) )
		{
			bHasRandomQuest = true;
		}
	}

	bool bWasPendingQuest = m_QuestManager.IsPendingQuest( code );

	if( !m_QuestManager.StartQuest( code, nStartQuestID ) )
	{
		SendQuestMessage( CHAT_QUEST_SYSTEM, this, "START|FAIL|NOT_STARTABLE|%d", rQuestBase.nQuestTextId );
		return false;
	}

	StructQuest * pQuest = m_QuestManager.FindQuest( code );

	if( !pQuest )
		return false;

	// 대기 중이었던 퀘스트를 시작하는 경우
	if( bWasPendingQuest )
	{
		DBQuery( new DB_UpdateQuest( this, GetPlayerUID(), pQuest->GetQuestID(),
			pQuest->GetStatus( 0 ), pQuest->GetStatus( 1 ), pQuest->GetStatus( 2 ), pQuest->GetStatus( 3 ), pQuest->GetStatus( 4 ), pQuest->GetStatus( 5 ),
			pQuest->GetRemainTime(), pQuest->GetProgress() ) );
	}
	// 대기 중이었던 퀘스트를 시작하지 않고 NPC 또는 스크립트를 통해 바로 시작하는 경우
	else
	{
		DBQuery( new DB_InsertQuest( this, GetPlayerUID(), pQuest->GetQuestID(), pQuest->GetQuestCode(), pQuest->GetQuestInstance().nStartID,
			pQuest->GetStatus( 0 ), pQuest->GetStatus( 1 ), pQuest->GetStatus( 2 ), pQuest->GetStatus( 3 ), pQuest->GetStatus( 4 ), pQuest->GetStatus( 5 ),
			pQuest->GetRemainTime(), pQuest->GetProgress() ) );
	}

	m_nLastAcceptQuest = code;

	onStartQuest( pQuest );

	if( pQuest->IsRandomQuest() )
	{
		if( bHasRandomQuest )
		{
			DBQuery( new DB_UpdateRandomQuest( this, GetPlayerUID(), pQuest->GetQuestCode(), pQuest->GetRandomKey( 0 ), pQuest->GetRandomKey( 1 ), pQuest->GetRandomKey( 2 ), pQuest->GetRandomValue( 0 ), pQuest->GetRandomValue( 1 ), pQuest->GetRandomValue( 2 ), 0 ) );

		}
		else
		{
			DBQuery( new DB_InsertRandomQuest( this, GetPlayerUID(), pQuest->GetQuestCode(), pQuest->GetRandomKey( 0 ), pQuest->GetRandomKey( 1 ), pQuest->GetRandomKey( 2 ), pQuest->GetRandomValue( 0 ), pQuest->GetRandomValue( 1 ), pQuest->GetRandomValue( 2 ) ) );
		}
	}

	SendQuestMessage( CHAT_QUEST_SYSTEM, this, "START|SUCCESS|%d", rQuestBase.nCode );
	SendQuestList( this );

	if( !pQuest->GetQuestBase().strAcceptScript.empty() )
	{
		char buf[256];
		s_sprintf( buf, _countof( buf ), "iocp.%d.script", GetCurrentThreadId() );
		ENV().Set( buf, pQuest->GetQuestBase().strAcceptScript.c_str() );

		ThreadPlayerHelper TPHelper( this );
		LUA()->RunString( pQuest->GetQuestBase().strAcceptScript.c_str() );
	}

	LOG::Log11N4S( LM_QUEST_START, GetAccountID(), GetSID(), 0, pQuest->GetQuestCode(), pQuest->GetQuestID(), 0, 0, 0, 0, 0, 0, GetAccountName(), LOG::STR_NTS, GetName(), LOG::STR_NTS, GameContent::GetString( rQuestBase.nQuestTextId ), LOG::STR_NTS, "", 0 );

	m_TitleManager.UpdateTitleConditionByQuestStart( pQuest->GetQuestCode() );

	return true;
}

const bool StructPlayer::AddPendingQuest( const QuestBase::QuestCode code, int nStartID )
{
	if( !m_QuestManager.AddPendingQuest( code, nStartID ) )
		return false;

	StructQuest * pQuest = m_QuestManager.FindQuest( code );

	if( !pQuest )
		return false;

	DBQuery( new DB_InsertQuest( this, GetPlayerUID(), pQuest->GetQuestID(), pQuest->GetQuestCode(), pQuest->GetQuestInstance().nStartID,
		pQuest->GetStatus( 0 ), pQuest->GetStatus( 1 ), pQuest->GetStatus( 2 ), pQuest->GetStatus( 3 ), pQuest->GetStatus( 4 ), pQuest->GetStatus( 5 ),
		pQuest->GetRemainTime(), pQuest->GetProgress() ) );

	LOG::Log11N4S( LM_QUEST_PENDING, GetAccountID(), GetSID(), 0, pQuest->GetQuestCode(), pQuest->GetQuestID(), 0, 0, 0, 0, 0, 0, GetAccountName(), LOG::STR_NTS, GetName(), LOG::STR_NTS, GameContent::GetString( pQuest->GetQuestBase().nQuestTextId ), LOG::STR_NTS, "", 0 );

	SendQuestList( this );
	return true;
}

bool StructPlayer::EndQuest( QuestBase::QuestCode code, int nSelectRewardIdx /* = -1 */ )
{
	ARCADIA_LOCK( ArcadiaServer::Instance().LockObjectWithVisibleRange( this ) );

	c_fixed10 fRewardRate;
	fRewardRate.set( 10000 );

	if( IsGameTimeLimited() )
	{
		if( GetContinuousPlayTime() >= GameRule::nMaxTiredGameTime )
		{
			SendQuestMessage( CHAT_QUEST_SYSTEM, this, "END|GAMETIME_NO_REWARD|%d", code );
			return false;
		}
		else if( GetContinuousPlayTime() >= GameRule::nMaxHealthyGameTime )
		{
			SendQuestMessage( CHAT_QUEST_SYSTEM, this, "END|GAMETIME_HALF_REWARD|%d", code );
			fRewardRate.set( 5000 );
		}
	}

	StructQuest* pQuest;
	int i;

	if( !CheckFinishableQuestAndGetQuestStruct( code, &pQuest ) )
	{
		SendQuestMessage( CHAT_QUEST_SYSTEM, this, "END|FAIL|%d", code );
		return false;
	}

	float fMod = 0.0f;
	if( pQuest->IsRandomQuest() )
	{
		for( int i = 0; i < QuestInstance::MAX_RANDOM_VALUE; ++i )
		{
			fMod += pQuest->GetValue( i * 4 + 3 ) * pQuest->GetRandomValue( i ) / 100.0f;
		}
	}
	else
	{
		fMod = 1.0f;
	}

	StructGold nRewardGold( fRewardRate * fMod * pQuest->GetGold().GetRawData() );
	StructGold nPrevGold( GetGold() );

	// 루피 지급 불가 확인을 위해 미리 지급(이후 처리 실패 시 nPrevGold로 롤백해야 함)
	if( ChangeGold( GetGold() + nRewardGold ) != RESULT_SUCCESS )
	{
		SendQuestMessage( CHAT_QUEST_SYSTEM, this, "END|TOO_MUCH_MONEY|%d", code );
		return false;
	}

	if( !m_QuestManager.EndQuest( pQuest ) )
	{
		if( ChangeGold( nPrevGold ) != RESULT_SUCCESS )
		{
			assert( 0 );
			_cprint( "ChangeGold/ChangeStorageGold Failed: Case[6], Player[%s], Info[Owned(%I64d), Target(%I64d)]\n", GetName(), GetGold().GetRawData(), nPrevGold.GetRawData() );
			FILELOG( "ChangeGold/ChangeStorageGold Failed: Case[6], Player[%s], Info[Owned(%I64d), Target(%I64d)]", GetName(), GetGold().GetRawData(), nPrevGold.GetRawData() );
		}

		SendQuestMessage( CHAT_QUEST_SYSTEM, this, "END|FAIL|%d", code );
		return false;
	}


	__int64 nRewardExp = pQuest->GetExp();
	bool bUseNegExp = nRewardExp < 0;
	if (bUseNegExp)
	{
		__int64 nLevelExp = GameContent::GetNeedExp(pQuest->GetQuestBase().nLimitLevel) - GameContent::GetNeedExp(pQuest->GetQuestBase().nLimitLevel - 1);
		nLevelExp *= nRewardExp;
		nRewardExp = nLevelExp / -100;
		SendQuestMessage(CHAT_QUEST_SYSTEM, this, "END|EXP|%d|%I64d|%d|%I64d|%d", pQuest->GetQuestCode(), nRewardExp, fRewardRate * pQuest->GetJP(), (fRewardRate * pQuest->GetGold().GetRawData()).GetAsInt64(), fRewardRate * pQuest->GetHuntaholicPoint());
	}
	else
	{
		SendQuestMessage(CHAT_QUEST_SYSTEM, this, "END|EXP|%d|%I64d|%d|%I64d|%d", pQuest->GetQuestCode(), (fRewardRate * pQuest->GetExp()).GetAsInt64(), fRewardRate * pQuest->GetJP(), (fRewardRate * pQuest->GetGold().GetRawData()).GetAsInt64(), fRewardRate * pQuest->GetHuntaholicPoint());
	}



	// 최대 레벨인 상태라면 경험치를 습득하지 못해야 함
	if( GameRule::nMaxLevel > 0 )
	{
		__int64 nMaxEXP = GameContent::GetNeedExp( GameRule::nMaxLevel ) - 1; // Epic 9.1 - MaxLevel 99.99%까지 경험치 얻을 수 있게 변경
		if( GetEXP() + nRewardExp > nMaxEXP )
		{
			if( GetEXP() >= nMaxEXP )
			{
				nRewardExp = 0;
			}
			else
			{
				nRewardExp = nMaxEXP - GetEXP();
			}
		}
	}

	int nRewardJP = fRewardRate * fMod * pQuest->GetJP();

	if( nRewardJP > 0 )
	{
		if( m_nJobPoint >= GameRule::MAX_OWNABLE_JP )
		{
			nRewardJP = 0;
			SendChatMessage( false, CHAT_EXP, "@SYSTEM", this, "@6759" );
		}
		else if( GameRule::MAX_OWNABLE_JP - m_nJobPoint < nRewardJP )
			nRewardJP = GameRule::MAX_OWNABLE_JP - m_nJobPoint;
	}

	// 여긴 소환수에게 나눠주지 말고 그냥 exp를 줘야 함 => StructPlayer::AddExp 가 호출되면 안 됨.
	if (!bUseNegExp)
	{
		StructCreature::AddExp(((fRewardRate* fMod)* nRewardExp).GetAsInt64(), nRewardJP, false);
	}
	else
	{
		StructCreature::AddExp(nRewardExp, nRewardJP, false);
	}

	// 보상 헌터홀릭 포인트 지급
	if( fRewardRate * pQuest->GetHuntaholicPoint() >= 1 )
	{
		SetHuntaholicPoint( GetHuntaholicPoint() + ( fRewardRate * pQuest->GetHuntaholicPoint() ) );
	}

	int favor_id = pQuest->GetQuestBase().nFavorGroupId;

	if( favor_id == QuestBaseServer::FAVOR_GROUP_ID_CONTACT )
	{
		StructNPC::iterator it = StructNPC::get( GetContactNPCHandle() );

		if( *it && (*it)->IsNPC() )
		{
			favor_id = static_cast< StructNPC * >( *it )->GetNPCID();
		}
		else
			favor_id = 0;
	}

	if( favor_id )
	{
		AddFavor( favor_id, fMod * pQuest->GetFavor() );
	}

	int hate_id = pQuest->GetQuestBase().nHateGroupId;

	if( hate_id == QuestBaseServer::FAVOR_GROUP_ID_CONTACT )
	{
		StructNPC::iterator it = StructNPC::get( GetContactNPCHandle() );

		if( *it && (*it)->IsNPC() )
		{
			hate_id = static_cast< StructNPC * >( *it )->GetNPCID();
		}
		else
			hate_id = 0;
	}

	if( hate_id )
	{
		AddFavor( hate_id, 0 - fMod * pQuest->GetFavor() );
	}

	// { 아이템 제거
	if( pQuest->GetQuestType() == QuestBase::QUEST_COLLECT ||
		pQuest->GetQuestType() == QuestBase::QUEST_HUNT_ITEM ||
		pQuest->GetQuestType() == QuestBase::QUEST_HUNT_ITEM_FROM_ANY_MONSTERS ||
		pQuest->GetQuestType() == QuestBase::QUEST_HUNT_ITEM_DROP_PENALTY )
	{
		int nMaxItemCollectTypeCount = ( pQuest->GetQuestType() == QuestBase::QUEST_COLLECT ) ? QuestInstance::MAX_STATUS : 3;

		for( i = 0 ; i < nMaxItemCollectTypeCount ; ++i ) // 하드코딩~
		{
			ItemBase::ItemCode	nItemCode = pQuest->GetValue( i * 2 );
			__int64				nQuantity( pQuest->GetValue( i * 2 + 1 ) );

			if( nItemCode == 0 )
			{
				continue;
			}

			StructItem * pItem = FindItem( nItemCode );

			if( !pItem )
			{
				// 원래 이런 상황이 발생하면 안되지만, 백섭등으로 Quest정보에 저장된 값이랑 Inven정보랑 다를 때,
				// 그냥 스킵힌다.
				continue;
			}

			EraseItem( pItem, nQuantity );
		}
	}
	// }

	// { 보상 아이템 지급
	if( pQuest->GetDefaultReward().nItemCode )
	{
		StructItem *pItem = StructItem::AllocItem( 0, pQuest->GetDefaultReward().nItemCode, ( fRewardRate * pQuest->GetDefaultReward().nQuantity ) + 0.5f, ItemInstance::BY_QUEST, std::max( pQuest->GetDefaultReward().nLevel, 1 ) );
		StructItem *pNewItem = PushItem( pItem, pItem->GetCount() );

		if( pNewItem )
			LOG::Log11N4S( LM_ITEM_TAKE, GetAccountID(), GetSID(), pItem->GetItemEnhance() * 100 + pItem->GetItemLevel(), pNewItem->GetItemCode(), pItem->GetCount(), pNewItem->GetCount(), pQuest->GetGold().GetRawData(), GetGold().GetRawData(), GetX(), GetY(), pNewItem->GetItemUID(), GetAccountName(), LOG::STR_NTS, GetName(), LOG::STR_NTS, "", 0, "QUEST", LOG::STR_NTS );

		SendQuestMessage( CHAT_QUEST_SYSTEM, this, "END|REWARD|%d", pItem->GetItemCode() );

		if( pNewItem != pItem ) 
		{
			StructItem::PendFreeItem( pItem );
		}
	}

	if( nSelectRewardIdx != -1 && nSelectRewardIdx < QuestBase::MAX_OPTIONAL_REWARD && pQuest->GetOptionalReward( nSelectRewardIdx ).nItemCode  )
	{
		StructItem *pItem = StructItem::AllocItem( 0, pQuest->GetOptionalReward( nSelectRewardIdx ).nItemCode, ( fRewardRate * pQuest->GetOptionalReward( nSelectRewardIdx ).nQuantity ) + 0.5f, ItemInstance::BY_QUEST, std::max( pQuest->GetDefaultReward().nLevel, 1 ) );
		StructItem *pNewItem = PushItem( pItem, pItem->GetCount() );

		if( pNewItem )
			LOG::Log11N4S( LM_ITEM_TAKE, GetAccountID(), GetSID(), pItem->GetItemEnhance() * 100 + pItem->GetItemLevel(), pNewItem->GetItemCode(), pItem->GetCount(), pNewItem->GetCount(), 0, 0, GetX(), GetY(), pNewItem->GetItemUID(), GetAccountName(), LOG::STR_NTS, GetName(), LOG::STR_NTS, "", 0, "QUEST", LOG::STR_NTS );

		SendQuestMessage( CHAT_QUEST_SYSTEM, this, "END|REWARD|%d", pItem->GetItemCode() );

		if( pNewItem != pItem ) 
		{
			StructItem::PendFreeItem( pItem );
		}
	}
	// }

	if( !pQuest->GetQuestBase().strClearScript.empty() )
	{
		ThreadPlayerHelper TPHelper( this );
		LUA()->RunString( pQuest->GetQuestBase().strClearScript.c_str() );
	}

	// { 디비에 반영하고 로그 남김
	DBQuery( new DB_UpdateQuest( this,
		GetPlayerUID(),
		pQuest->GetQuestID(),
		pQuest->GetStatus( 0 ),
		pQuest->GetStatus( 1 ),
		pQuest->GetStatus( 2 ),
		pQuest->GetStatus( 3 ),
		pQuest->GetStatus( 4 ),
		pQuest->GetStatus( 5 ),
		pQuest->GetRemainTime(),
		pQuest->GetProgress() ) );

	LOG::Log11N4S( LM_QUEST_CLEAR, GetAccountID(), GetSID(), 0, pQuest->GetQuestCode(), pQuest->GetQuestID(), 0, 0, 0, 0, 0, 0, GetAccountName(), LOG::STR_NTS, GetName(), LOG::STR_NTS, GameContent::GetString( pQuest->GetQuestBase().nQuestTextId ), LOG::STR_NTS, "", 0 );
	// }

	// ActiveQuestList에서 제거하고 주변 NPC StatusFlag 갱신
	onEndQuest( pQuest );

	SendQuestList( this );

	Save();

	m_TitleManager.UpdateTitleConditionByQuestEnd( code );

	return true;
}

bool StructPlayer::DropQuest( QuestBase::QuestCode code )
{
	ARCADIA_LOCK( ArcadiaServer::Instance().LockObject( this ) );

	StructQuest * pQuest = m_QuestManager.FindQuest( code );

	if( !pQuest )
		return false;

	DBQuery( new DB_DeleteQuest( this, GetPlayerUID(), pQuest->GetQuestID() ) );
	if( pQuest->IsRandomQuest() )
	{
		DBQuery( new DB_UpdateRandomQuest( this, GetPlayerUID(), pQuest->GetQuestCode(), pQuest->GetRandomKey( 0 ), pQuest->GetRandomKey( 1 ), pQuest->GetRandomKey( 2 ), pQuest->GetRandomValue( 0 ), pQuest->GetRandomValue( 1 ), pQuest->GetRandomValue( 2 ), 1 ) );
		m_QuestManager.SetDropFlagToRandomQuestInfo( pQuest->GetQuestCode() );
	}

	LOG::Log11N4S( LM_QUEST_CANCEL, GetAccountID(), GetSID(), 0, pQuest->GetQuestCode(), pQuest->GetQuestID(), pQuest->GetProgress(), 0, 0, 0, 0, 0, GetAccountName(), LOG::STR_NTS, GetName(), LOG::STR_NTS, GameContent::GetString( pQuest->GetQuestBase().nQuestTextId ), LOG::STR_NTS, "", 0 );

	onDropQuest( pQuest );

	SendQuestList( this );

	return true;
}

const bool StructPlayer::ResetFinishedQuest( QuestBase::QuestCode code )
{
	ARCADIA_LOCK( ArcadiaServer::Instance().LockObject( this ) );

	if ( QuestInstance::QuestID id = m_QuestManager.GetFinishedQuestID( code ) )
	{
		DBQuery( new DB_DeleteQuest( this, GetPlayerUID(), id ) );
		m_QuestManager.ResetFinishedQuest( code );
	}

	// NPC 머리 위에 표시 갱신 클라 방송
	SendNPCStatusInVisibleRange( this );

	return true;
}

int	StructPlayer::GetQuestProgress( QuestBase::QuestCode code ) const
{
	if( m_QuestManager.IsStartableQuest( code ) ) return QuestInstance::NOT_STARTED;

	if( m_QuestManager.IsFinishedQuest( code ) ) return QuestInstance::FINISHED;

	StructQuest * pQuest = m_QuestManager.FindQuest( code );

	if( !pQuest ) return -1;

	if( pQuest->IsFinishable() ) return QuestInstance::FINISHABLE;

	return pQuest->GetProgress();
}

const int StructPlayer::GetQuestStartTextID( QuestBase::QuestCode code ) const
{
	StructQuest * pQuest = m_QuestManager.FindQuest( code );

	if( !pQuest )
		return 0;

	return pQuest->GetQuestInstance().nStartID;
}

const bool StructPlayer::IsStartableQuest( QuestBase::QuestCode code ) const
{
	const QuestBaseServer & rQuestBase = StructQuest::GetQuestBase( code );

	if( rQuestBase.nBeginTime || rQuestBase.nEndTime )
	{
		SYSTEMTIME stSystemTime;
		GetLocalTime( &stSystemTime );
		int nSecondsAfterMidnight = stSystemTime.wHour * 3600 + stSystemTime.wMinute * 60 + stSystemTime.wSecond;

		if( rQuestBase.nBeginTime > nSecondsAfterMidnight || nSecondsAfterMidnight > rQuestBase.nEndTime )
			return false;
	}

	if( rQuestBase.nLimitLevel > GetLevel() || ( rQuestBase.nLimitMaxLevel && rQuestBase.nLimitMaxLevel < GetLevel() ) )
	{
		return false;
	}

	if( rQuestBase.nLimitJobLevel > GetJobLevel() || ( rQuestBase.nLimitMaxJobLevel && rQuestBase.nLimitMaxJobLevel < GetJobLevel() ) )
	{
		return false;
	}

	// 직업 차수 체크
	if( ( 1 << GetJobDepth() & rQuestBase.nJobDepth ) == 0 ) 
	{
		return false;
	}

	if( rQuestBase.nLimitJob != 0 )
	{
		if( rQuestBase.nLimitJob != GetJobId() )
		{
			return false;
		}
	}
	else
	{
		while( true )
		{
			if( IsHunter() )	if( rQuestBase.LimitFlag.IsOn( QuestBase::LIMIT_HUNTER ) ) break;
			if( IsFighter() )	if( rQuestBase.LimitFlag.IsOn( QuestBase::LIMIT_FIGHTER ) ) break;
			if( IsMagician() )	if( rQuestBase.LimitFlag.IsOn( QuestBase::LIMIT_MAGICIAN ) ) break;
			if( IsSummoner() )	if( rQuestBase.LimitFlag.IsOn( QuestBase::LIMIT_SUMMONER ) ) break;

			return false;
		}
	}

	while( true )
	{
		if( GetRace() == JobInfo::GAIA )	if( rQuestBase.LimitFlag.IsOn( QuestBase::LIMIT_GAIA ) ) break;
		if( GetRace() == JobInfo::DEVA )	if( rQuestBase.LimitFlag.IsOn( QuestBase::LIMIT_DEVA ) ) break;
		if( GetRace() == JobInfo::ASURA )	if( rQuestBase.LimitFlag.IsOn( QuestBase::LIMIT_ASURA ) ) break;

		return false;
	}

	int favor_id = rQuestBase.nLimitFavorGroupId;

	if( favor_id == QuestBaseServer::FAVOR_GROUP_ID_CONTACT )
	{
		StructNPC::iterator it = StructNPC::get( GetContactNPCHandle() );

		if( *it && (*it)->IsNPC() )
		{
			favor_id = static_cast< StructNPC * >( *it )->GetNPCID();
		}
		else
			favor_id = 0;
	}

	if( favor_id )
	{
		if( GetFavor( favor_id ) < rQuestBase.nLimitFavor )
			return false;
	}

	return m_QuestManager.IsStartableQuest( code );
}

// 실패한 퀘스트도 일단 진행 상태로 반환(퀘스트 시작 불가이며, NPC 대사 또는 정보 텍스트를 진행 중 텍스트로 출력)
const bool StructPlayer::IsInProgressQuest( QuestBase::QuestCode code ) const
{
	StructQuest * pQuest = m_QuestManager.FindQuest( code );

	if( !pQuest )
	{
		return false;
	}

	if( pQuest->IsFinishable() )
	{
		return false;
	}

	return true;
}

void StructPlayer::UpdateQuestStatusByItemUpgrade()
{
	std::vector< StructQuest* >::iterator it;
	std::vector< StructQuest* > vQuestList;
	m_QuestManager.GetRelatedQuest( vQuestList, QuestBase::TYPE_FLAG_UPGRADE_ITEM );

	if( vQuestList.empty() ) return;

	for( it = vQuestList.begin(); it != vQuestList.end(); ++ it )
	{
		bool bFinishable = (*it)->IsFinishable();

		for( int nIndex = 0 ; nIndex < QuestBase::MAX_VALUE_NUMBER ; nIndex += 2 )
		{
			int nValueIdx = nIndex + 1;
			if( (*it)->GetValue( nValueIdx ) > 0 && (*it)->GetValue( nIndex ) < ItemBase::MAX_ITEM_WEAR )
			{
				StructItem * pItem = GetWearedItem( (ItemBase::ItemWearType)(int)(*it)->GetValue( nIndex ) );
				(*it)->UpdateStatus( nIndex / 2, pItem ? std::min( pItem->GetItemLevel(), (int) (*it)->GetValue( nValueIdx ) ) : 0 );
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
	}
}

const bool StructPlayer::IsTakeableQuestItem( QuestBase::QuestCode code ) const
{
	return m_QuestManager.IsTakeableQuestItem( code );
}

const bool StructPlayer::IsFinishableQuest( QuestBase::QuestCode code ) const
{
	return CheckFinishableQuestAndGetQuestStruct( code, NULL );
}

const bool StructPlayer::CheckFinishableQuestAndGetQuestStruct( QuestBase::QuestCode code, StructQuest** pQuest ) const
{
	StructQuest* pQuestStruct = m_QuestManager.FindQuest( code );

	if( !pQuestStruct )
	{
		return false;
	}

	// 여기서는 순전히 StructQuest의 Status만 보고 검사한다.
	// 수집퀘의 경우, 인벤토리를 다시 한번 확인해주는 센스가 필요하다.
	// 물론, 완료시 인벤에서 해당 아이템 지워주는 센스도 필요하다.
	if( pQuestStruct->IsFinishable() )
	{
		if( pQuest )
		{
			(*pQuest) = pQuestStruct;
		}
		return true;
	}

	return false;
}

void StructPlayer::onProgressChanged( struct StructQuest *pQuest, QuestInstance::QUEST_PROGRESS oldProgress, QuestInstance::QUEST_PROGRESS newProgress )
{
	// 실패로 인해 진행이 불가능해 진 퀘스트인 경우에는 퀘스트 정보 방송을 자동적으로 해 줌(그 외에는 대개 onStatusChanged에 의해 방송이 이루어 짐)
	if( newProgress == QuestInstance::FAIL )
	{
		SendNPCStatusInVisibleRange( this );

		SendQuestStatus( this, pQuest );
	}
}

void StructPlayer::onStatusChanged( struct StructQuest *pQuest )
{
	if( pQuest->IsFinishable() )
		SendNPCStatusInVisibleRange( this );

	SendQuestStatus( this, pQuest );
}

void StructPlayer::onTitleAchieved( StructTitle * pTitle )
{
	if( pTitle->GetDBInsertFlag() )
	{
		DBQuery( new DB_InsertTitle( this, pTitle->GetSID(), GetPlayerUID(), pTitle->GetCode(), pTitle->GetStatus() ) );
		pTitle->SetDBInsertFlag( false );
	}
	else
	{
		DBQuery( new DB_UpdateTitle( this, pTitle->GetSID(), pTitle->GetStatus() ) );
		pTitle->SetDBUpdateFlag( false );
	}

	TS_SC_ACHIEVE_TITLE msg;
	msg.code = pTitle->GetCode();
	PendMessage( this, &msg );

	LOG::Log11N4S( LM_TITLE_ACHIEVE, GetAccountID(), GetSID(), pTitle->GetSID(), pTitle->GetCode(), pTitle->GetStatus(), 0, 0, 0, 0, 0, 0, GetAccountName(), LOG::STR_NTS, GetName(), LOG::STR_NTS, GameContent::GetString( StructTitleManager::GetTitleBase( pTitle->GetCode() )->nNameID ), LOG::STR_NTS, "", 0 );
}

void StructPlayer::onTitleOpened( StructTitle * pTitle )
{
	if( pTitle->GetDBInsertFlag() )
	{
		DBQuery( new DB_InsertTitle( this, pTitle->GetSID(), GetPlayerUID(), pTitle->GetCode(), pTitle->GetStatus() ) );
		pTitle->SetDBInsertFlag( false );
	}
	else
	{
		DBQuery( new DB_UpdateTitle( this, pTitle->GetSID(), pTitle->GetStatus() ) );
		pTitle->SetDBUpdateFlag( false );
	}

	TS_SC_OPEN_TITLE msg;
	msg.code = pTitle->GetCode();
	PendMessage( this, &msg );

	LOG::Log11N4S( LM_TITLE_OPEN, GetAccountID(), GetSID(), pTitle->GetSID(), pTitle->GetCode(), pTitle->GetStatus(), 0, 0, 0, 0, 0, 0, GetAccountName(), LOG::STR_NTS, GetName(), LOG::STR_NTS, GameContent::GetString( StructTitleManager::GetTitleBase( pTitle->GetCode() )->nNameID ), LOG::STR_NTS, "", 0 );
}

void StructPlayer::onTitleBookmarked( StructTitle * pTitle )
{
	TS_SC_BOOKMARK_TITLE msg;
	msg.code = pTitle->GetCode();
	msg.bookmarked = pTitle->GetStatus() & StructTitle::TITLE_STATUS_BOOKMARK;
	PendMessage( this, &msg );
}

void StructPlayer::onConditionChanged( struct TitleConditionType * pCondition, const __int64 nCount )
{
	TS_SC_CHANGE_TITLE_CONDITION msg;
	msg.condition_id = pCondition->nID;
	msg.count = nCount;
	PendMessage( this, &msg );
}

void StructPlayer::GetQuestByMonster( int nMonsterId, std::vector< StructQuest* > & vQuest, int type )
{
	return m_QuestManager.GetRelatedQuestByMonster( nMonsterId, vQuest, type );
}

void StructPlayer::UpdateQuestStatusByMonsterKill( int nMonsterId )
{
	m_QuestManager.UpdateQuestStatusByMonsterKill( nMonsterId );
}

bool StructPlayer::IsUsableTitle( int nCode )
{
	// 착용 해제는 무조건 가능
	if( !nCode )
		return true;

	// 미획득 호칭 착용 시도
	if( nCode && !IsAchievedTitle( nCode ) )
		return false;

	// 이미 착용하고 있는 호칭
	if( GetMainTitle() && GetMainTitle()->nID == nCode )
		return false;

	for( int i = 0; i < GameRule::SUB_TITLE_COUNT; i++ )
	{
		const TitleBaseServer * pSubTitle = GetSubTitle( i );
		
		if( !pSubTitle )
			continue;

		if( pSubTitle->nID == nCode )
			return false;
	}

	return true;
}

bool StructPlayer::SetMainTitle( int nCode )
{
	AR_TIME t = GetArTime();
	
	// 쿨타임 5분
	if( m_pMainTitle && t < GetRemainTitleTime() )
			return false;

	if( !IsUsableTitle( nCode ) )
		return false;

	// nCode가 0이면 해제 요청
	if( nCode )
	{
		m_pMainTitle = StructTitleManager::GetTitleBase( nCode );

		SetRemainTitleTime( t + GameRule::TITLE_RESET_COOL_TIME );
		SendRemainTitleTime( this );
	}
	else
	{
		m_pMainTitle = NULL;
	}

	CalculateStat();

	// 방송
	TS_SC_SET_MAIN_TITLE msg;
	msg.handle = GetHandle();
	msg.code = nCode;
	ArcadiaServer::Instance().Broadcast( GetRX(), GetRY(), GetLayer(), &msg );

	return true;
}

bool StructPlayer::SetSubTitle( int nIndex, int nCode )
{
	if( !IsUsableTitle( nCode ) )
		return false;

	// nCode가 0이면 해제 요청
	if( nCode )
	{
		TitleBaseServer * pTitle = StructTitleManager::GetTitleBase( nCode );

		// 부 호칭에서 착용 가능한 최대 등급은 5등급이다.
		if( pTitle->nRate > GameRule::SUB_TITLE_RATE_LIMIT )
			return false;

		m_pSubTitle[nIndex] = pTitle;
	}
	else
	{
		m_pSubTitle[nIndex] = NULL;
	}

	CalculateStat();

	TS_SC_SET_SUB_TITLE msg;
	msg.index = nIndex;
	msg.code = nCode;
	PendMessage( this, &msg );

	return true;
}

bool StructPlayer::OpenTitle( int nCode )
{
	m_TitleManager.OpenTitle( nCode );

	return true;
}

bool StructPlayer::AchieveTitle( int nCode )
{
	m_TitleManager.AchieveTitle( nCode );

	return true;
}

bool StructPlayer::BookmarkTitle( int nCode )
{
	m_TitleManager.BookmarkTitle( nCode );

	return true;
}

bool StructPlayer::OpenBooth( const char * szBoothName, const std::vector< StructPlayer::BOOTH_OPEN_ITEM_INFO > & vItemList, bool bIsBuyBooth )
{
	if( GetBoothStatus() != IS_NOT_BOOTH || m_pBoothOpener )
		return false;

	std::vector< BOOTH_OPEN_ITEM_INFO >::const_iterator it;

	m_vBoothItem.clear();

	for( it = vItemList.begin(); it != vItemList.end(); it++ )
	{
		BOOTH_ITEM_INFO BoothItemInfo;

		BoothItemInfo.pItem = StructItem::FindItem( (*it).handle );
		BoothItemInfo.cnt = (*it).cnt;
		BoothItemInfo.gold = (*it).gold;

		if( !BoothItemInfo.pItem )
		{
			m_vBoothItem.clear();
			return false;
		}

		if( !IsTradable( BoothItemInfo.pItem ) )
		{
			m_vBoothItem.clear();
			return false;
		}

		if( !bIsBuyBooth && ( BoothItemInfo.pItem->GetCount() < (*it).cnt || (*it).cnt <= 0 ) )
		{
			m_vBoothItem.clear();
			return false;
		}

		// 구매 노점일경우 중첩 불가 아이템은 최대 1개 구매 가능.
		if( bIsBuyBooth && !BoothItemInfo.pItem->IsJoinable() && BoothItemInfo.cnt != 1 )
		{
			m_vBoothItem.clear();
			return false;
		}
		
		if( (*it).gold <= 0 )
		{
			m_vBoothItem.clear();
			return false;
		}

		// 한번이라도 사용된 크리처 카드일경우(SID 존재) 구매노점에 등록 불가능하다.
		if( bIsBuyBooth && BoothItemInfo.pItem->IsSummonCard() && BoothItemInfo.pItem->GetSummonSID() )
		{
			m_vBoothItem.clear();
			return false;
		}

		m_vBoothItem.push_back( BoothItemInfo );
	}

	m_strBoothName = szBoothName;

	XStringUtil::Replace( m_strBoothName , "<", "(" );
	XStringUtil::Replace( m_strBoothName , ">", ")" );

	m_BoothStatus = bIsBuyBooth ? BUY_BOOTH : SELL_BOOTH;

	if( m_pBoothOpener )
	{
		SendBoothClosedMessage( this, m_pBoothOpener->GetHandle() );
		StopWatchBooth();
	}

	return true;
}

bool StructPlayer::CloseBooth()
{
	if( GetBoothStatus() == IS_NOT_BOOTH )
		return false;

	m_BoothStatus = IS_NOT_BOOTH;

	m_strBoothName = "";
	m_vBoothItem.clear();

	// m_vBoothWatcher를 직접 순회하면서 pWatcher->StopWatchBooth() 호출하면 m_vBoothWatcher가 변경되며 it가 무효화됨
	// 그래서 사본을 생성해서 사본을 순회하며 처리함
	std::vector< StructPlayer * > vBoothWatcher;
	vBoothWatcher.swap( m_vBoothWatcher );
	for( std::vector< StructPlayer * >::iterator it = vBoothWatcher.begin(); it != vBoothWatcher.end(); ++it )
	{
		StructPlayer * pWatcher = (*it);
		SendBoothClosedMessage( pWatcher, GetHandle() );
		pWatcher->StopWatchBooth();
	}

	return true;
}

const char * StructPlayer::GetBoothName()
{
	return m_strBoothName.c_str();
}

const std::vector< StructPlayer::BOOTH_ITEM_INFO > & StructPlayer::GetBoothItemList()
{
	return m_vBoothItem;
}

void StructPlayer::WatchBooth( StructPlayer * pBooth )
{
	if( m_pBoothOpener )
	{
		StopWatchBooth();
	}

	m_pBoothOpener = pBooth;

	m_pBoothOpener->addToBoothWatcher( this );
}

void StructPlayer::StopWatchBooth()
{
	if( m_pBoothOpener )
	{
		m_pBoothOpener->removeFromBoothWatcher( this );
		m_pBoothOpener = NULL;
	}
}


void StructPlayer::addToBoothWatcher( StructPlayer * pWatcher )
{
	if( std::find( m_vBoothWatcher.begin(), m_vBoothWatcher.end(), pWatcher ) != m_vBoothWatcher.end() )
		return;

	m_vBoothWatcher.push_back( pWatcher );
}

void StructPlayer::removeFromBoothWatcher( StructPlayer * pWatcher )
{
	std::vector< StructPlayer * >::iterator it;

	it = std::find( m_vBoothWatcher.begin(), m_vBoothWatcher.end(), pWatcher );

	if( it != m_vBoothWatcher.end() )
		vector_fast_erase( &m_vBoothWatcher, it );
}

bool StructPlayer::BuyFromBooth( StructPlayer * pBuyer, std::vector< StructPlayer::BOOTH_ITEM_BUY_INFO > & vBuyInfo )
{
	std::vector< StructPlayer::BOOTH_ITEM_BUY_INFO >::iterator it;
	std::vector< StructItem * > vEraseList;
	std::vector< StructItem * >::iterator itDel;
	std::vector< StructPlayer::BOOTH_ITEM_INFO >::iterator itFind;

	for( it = vBuyInfo.begin(); it != vBuyInfo.end(); ++it )
	{
		BOOTH_ITEM_INFO & rItemInfo = m_vBoothItem[(*it).nIdx];

		// 로그를 위해 물건 전달 전에 줄 아이템 관련 로그 정보를 미리 남겨야 함(받은 건 전달 후에)
		int nItemUID = rItemInfo.pItem->GetItemUID();
		__int64 nItemCount = rItemInfo.pItem->GetCount();
		int nItemEnhanceLevel = rItemInfo.pItem->GetItemEnhance() * 100 + rItemInfo.pItem->GetItemLevel();

		// 돈 받고(루피 소지 한도로 인한 오류는 발생하지 않아야 함 - 이미 외부에서 체크 완료)
		StructGold nPrice( rItemInfo.gold * (*it).nCount ), nPrevBuyerGold( pBuyer->GetGold() );
		if( pBuyer->ChangeGold( pBuyer->GetGold() - nPrice ) != RESULT_SUCCESS || ChangeGold( GetGold() + nPrice ) != RESULT_SUCCESS )
		{
			if( pBuyer->ChangeGold( nPrevBuyerGold ) != RESULT_SUCCESS )
			{
				assert( 0 );
				_cprint( "ChangeGold/ChangeStorageGold Failed: Case[7], Player[%s], Info[Owned(%I64d), Target(%I64d)]\n", pBuyer->GetName(), pBuyer->GetGold().GetRawData(), nPrevBuyerGold.GetRawData() );
				FILELOG( "ChangeGold/ChangeStorageGold Failed: Case[7], Player[%s], Info[Owned(%I64d), Target(%I64d)]", pBuyer->GetName(), pBuyer->GetGold().GetRawData(), nPrevBuyerGold.GetRawData() );
			}

			// 여기 들어온다는 건 판매자 소지 루피 한도 초과
			break;
		}

		// 물건 주고
		AR_HANDLE hResultItem = 0;
		if( !GiveItem( pBuyer, rItemInfo.pItem->GetHandle(), (*it).nCount, &hResultItem ) )
		{
			continue;
		}

		// n10은 BuyFromBooth면 1, SellToBooth이면 2로 사용. 로그 추적용.
		LOG::Log11N4S( LM_BOOTH_SELL, GetAccountID(), GetSID(), nItemEnhanceLevel, rItemInfo.pItem->GetItemCode(), (*it).nCount, nItemCount, rItemInfo.gold * (*it).nCount, GetGold().GetRawData(), pBuyer->GetPlayerUID(), 1, nItemUID, GetAccountName(), LOG::STR_NTS, GetName(), LOG::STR_NTS, pBuyer->GetName(), LOG::STR_NTS, "BY_FROM_BOOTH", LOG::STR_NTS );

		StructItem * pReceivedItem = static_cast< StructItem * >( *GameObject::get( hResultItem ) );

		assert( pReceivedItem );

		if( pReceivedItem )
		{
			nItemUID = pReceivedItem->GetItemUID();
			nItemCount = pReceivedItem->GetCount();
			nItemEnhanceLevel = pReceivedItem->GetItemEnhance() * 100 + pReceivedItem->GetItemLevel();
		}

		// n10은 BuyFromBooth면 1, SellToBooth이면 2로 사용. 로그 추적용.
		LOG::Log11N4S( LM_BOOTH_BUY, pBuyer->GetAccountID(), pBuyer->GetSID(), nItemEnhanceLevel, rItemInfo.pItem->GetItemCode(), (*it).nCount, nItemCount, rItemInfo.gold * (*it).nCount, pBuyer->GetGold().GetRawData(), GetPlayerUID(), 1, nItemUID, pBuyer->GetAccountName(), LOG::STR_NTS, pBuyer->GetName(), LOG::STR_NTS, GetName(), LOG::STR_NTS, "SELL_TO_BOOTH", LOG::STR_NTS );

		if( rItemInfo.cnt == (*it).nCount )
		{
			vEraseList.push_back( rItemInfo.pItem );
			//m_vBoothItem.erase( m_vBoothItem.begin() + (*it).nIdx );
		}
		else
		{
			rItemInfo.cnt -= (*it).nCount;
		}
	}

	for( itDel = vEraseList.begin(); itDel != vEraseList.end(); ++itDel )
	{
		for( itFind = m_vBoothItem.begin(); itFind != m_vBoothItem.end(); ++itFind )
		{
			if( (*itFind).pItem == (*itDel) )
			{
				vector_fast_erase( &m_vBoothItem, itFind );
				break;
			}
		}
	}

	broadcastBoothInfo();

	return true;
}

bool StructPlayer::SellToBooth( StructPlayer * pSeller, AR_HANDLE hItem, int nIdx, const __int64 & nCnt )
{
	if( nIdx < 0 || nIdx >= static_cast<int>( m_vBoothItem.size() ) )
		return false;

	BOOTH_ITEM_INFO & rItemInfo = m_vBoothItem[nIdx];

	if( GetGold() < ( rItemInfo.gold * nCnt ) )
	{
		return false;
	}

	// 돈 주고(루피 소지 한도로 인한 오류는 발생하지 않아야 함 - 이미 외부에서 체크 완료)
	StructGold nPrice( rItemInfo.gold * nCnt ), nPrevSellerGold( pSeller->GetGold() );
	if( pSeller->ChangeGold( pSeller->GetGold() + nPrice ) != RESULT_SUCCESS || ChangeGold( GetGold() - nPrice ) != RESULT_SUCCESS )
	{
		if( pSeller->ChangeGold( nPrevSellerGold ) != RESULT_SUCCESS )
		{
			assert( 0 );
			_cprint( "ChangeGold/ChangeStorageGold Failed: Case[8], Player[%s], Info[Owned(%I64d), Target(%I64d)]\n", pSeller->GetName(), pSeller->GetGold().GetRawData(), nPrevSellerGold.GetRawData() );
			FILELOG( "ChangeGold/ChangeStorageGold Failed: Case[8], Player[%s], Info[Owned(%I64d), Target(%I64d)]", pSeller->GetName(), pSeller->GetGold().GetRawData(), nPrevSellerGold.GetRawData() );
		}

		// 여기 들어온다는 건 판매자 소지 루피 한도 초과
		return false;
	}

	// 로그를 위해 물건 전달 전에 줄 아이템 관련 로그 정보를 미리 남겨야 함(받은 건 전달 후에)
	int nItemUID = rItemInfo.pItem->GetItemUID();
	__int64 nItemCount = rItemInfo.pItem->GetCount();
	int nItemEnhanceLevel = rItemInfo.pItem->GetItemEnhance() * 100 + rItemInfo.pItem->GetItemLevel();

	// 물건 받고
	AR_HANDLE hResultItem;

	pSeller->GiveItem( this, hItem, nCnt, &hResultItem );

	// n10은 BuyFromBooth면 1, SellToBooth이면 2로 사용. 로그 추적용.
	LOG::Log11N4S( LM_BOOTH_SELL, pSeller->GetAccountID(), pSeller->GetSID(), nItemEnhanceLevel, rItemInfo.pItem->GetItemCode(), nCnt, nItemCount, rItemInfo.gold * nCnt, pSeller->GetGold().GetRawData(), GetPlayerUID(), 2, nItemUID, pSeller->GetAccountName(), LOG::STR_NTS, pSeller->GetName(), LOG::STR_NTS, GetName(), LOG::STR_NTS, "SELL_TO_BOOTH", LOG::STR_NTS );

	StructItem * pReceivedItem = static_cast< StructItem * >( *GameObject::get( hResultItem ) );

	assert( pReceivedItem );

	if( pReceivedItem )
	{
		nItemUID = pReceivedItem->GetItemUID();
		nItemCount = pReceivedItem->GetCount();
		nItemEnhanceLevel = pReceivedItem->GetItemEnhance() * 100 + pReceivedItem->GetItemLevel();
	}

	// n10은 BuyFromBooth면 1, SellToBooth이면 2로 사용. 로그 추적용.
	LOG::Log11N4S( LM_BOOTH_BUY, GetAccountID(), GetSID(), nItemEnhanceLevel, rItemInfo.pItem->GetItemCode(), nCnt, nItemCount, rItemInfo.gold * nCnt, GetGold().GetRawData(), pSeller->GetPlayerUID(), 2, nItemUID, GetAccountName(), LOG::STR_NTS, GetName(), LOG::STR_NTS, pSeller->GetName(), LOG::STR_NTS, "BUY_FROM_BOOTH", LOG::STR_NTS );

	if( nCnt == rItemInfo.cnt )
	{
		m_vBoothItem.erase( m_vBoothItem.begin() + nIdx );
	}
	else
	{
		rItemInfo.cnt -= nCnt;
	}

	broadcastBoothInfo();

	return true;
}

static XCriticalSection s_BoothLock;

XCriticalSection* StructPlayer::GetBoothLock()
{
	return &s_BoothLock;
}

void StructPlayer::broadcastBoothInfo()
{
	std::vector< StructPlayer * >::iterator it;

	for( it = m_vBoothWatcher.begin(); it != m_vBoothWatcher.end(); ++it )
	{
		SendBoothInfo( (*it), this );
	}
}

const unsigned short StructPlayer::IsBoothOpenable()
{
	if( !IsActable() || IsUsingSkill() || IsAttacking() || IsRiding() || HasRidingState() ) return RESULT_NOT_ACTABLE;
	if( !m_pWorldLocation || m_pWorldLocation->location_type != StructWorldLocation::LOCATION_MARICAT_MARKET ) return RESULT_NOT_ACTABLE_HERE;
	return RESULT_SUCCESS;
}

unsigned short StructPlayer::putonItem( ItemBase::ItemWearType pos , struct StructItem * pItem )
{
	if( pos == ItemBase::WEAR_LEFTHAND && pItem->IsWeapon() )
	{
		m_StatusFlag.On( STATUS_USING_DOUBLE_WEAPON );
	}

	unsigned short nRet = StructCreature::putonItem( pos, pItem );
	if( nRet != RESULT_SUCCESS )
		return nRet;

	// 착용된 아이템은 더 이상 무게에 포함되지 않으므로 뺀다
	if( m_anWear[ pos ] == pItem )
		m_Inventory.AddWeightModifier( -pItem->GetWeight() );

	UpdateWeightWithInventory();
	UpdateQuestStatusByItemUpgrade();
	// 장착 위치를 확인하지 않는다면 스왑용 장비에 의해 두 번 이상 장착되거나 장착 해제되는 경우가 발생할 수 있다.
	if( pos < ItemBase::MAX_ITEM_WEAR )
		UpdateTitleConditionByItemEquip( pItem->GetItemCode(), true );

	// 아이템 착용에 의한 스크립트 실행
	if( !pItem->GetItemBase().strScript.empty() && pos < ItemBase::MAX_ITEM_WEAR )
	{
		std::string strScript( pItem->GetItemBase().strScript );

		const char * pszProcScript = "on_equip_item";
		// 스크립트 내용에 on_equip_item이 있을 경우 파라미터 추가
		if( strstr( strScript.c_str(), pszProcScript ) )
		{
			// on_equip_item( 아이템 코드, 착용자 타입, 착용자 핸들, [소환수인 경우에만]Summon 코드값, [소환수인 경우에만]주인 핸들, 착용 위치 )
			std::string strOnEquipItem;
			XStringUtil::Format( strOnEquipItem, "on_equip_item( %d, %d, %u, %d, %d, %d )",
				pItem->GetItemCode(), StructItem::TARGET_TYPE_PLAYER, GetHandle(), 0, 0, pos );

			XStringUtil::Replace( strScript, pszProcScript, strlen( pszProcScript ), strOnEquipItem.c_str(), strOnEquipItem.length() );
		}

		// 일부 스크립트 함수에 의한 기능에서 대화 중인 NPC 핸들 값에 의해 제어되는 부분이 있으므로
		// 아이템에 의한 스크립트 실행 동안은 대화 중인 NPC 핸들을 0으로 설정함
		AR_HANDLE nContactNPCHandle = GetContactNPCHandle();
		SetContactNPCHandle( 0 );

		LUA()->RunString( strScript.c_str() );

		// 대화 중인 NPC 핸들 값을 원래대로 되돌림
		SetContactNPCHandle( nContactNPCHandle );
	}

	return RESULT_SUCCESS;
}

unsigned short StructPlayer::putoffItem( ItemBase::ItemWearType pos )
{
	if( !m_anWear[ pos ] )
		return RESULT_NOT_EXIST;

	if( pos == ItemBase::WEAR_ARMULET && GetChaos() )
		return RESULT_ACCESS_DENIED;

	if( pos == ItemBase::WEAR_LEFTHAND )
	{
		if( m_anWear[ pos ]->IsWeapon() )
		{
			m_StatusFlag.Off( STATUS_USING_DOUBLE_WEAPON );
		}

		if( m_anWear[ ItemBase::WEAR_DECO_SHIELD ] )
		{
			putoffItem( ItemBase::WEAR_DECO_SHIELD );
		}
	}
	else if( pos == ItemBase::WEAR_RIGHTHAND )
	{
		if( IsUsingDoubleWeapon() )
		{
			putoffItem( ItemBase::WEAR_LEFTHAND );
		}

		putoffItem( ItemBase::WEAR_DECO_WEAPON );
	}
	else if( pos == ItemBase::WEAR_DECO_WEAPON && IsUsingDoubleWeapon() && m_anWear[ ItemBase::WEAR_DECO_SHIELD ] )
	{
		putoffItem( ItemBase::WEAR_DECO_SHIELD );
	}
	else if( pos == ItemBase::WEAR_BELT )
	{
		bool bNeedSendBeltState = false;
		for( int i = 0 ; i < 8 ; ++i )
		{
			if( GetBeltSlotCardAt( i ) )
			{
				bNeedSendBeltState = true;
				PutOffBelt( i );
			}
		}

		if( bNeedSendBeltState )
			SendBeltSlotInfo( this );
	}

	if( pos == ItemBase::WEAR_RIDE_ITEM && m_nRidingStateCode )
	{
		RemoveState( m_nRidingStateCode );
	}

	StructItem *pItem = m_anWear[ pos ];

	unsigned short nRet = StructCreature::putoffItem( pos );

	// 착용되지 않은 아이템은 무게에 다시 포함되어야 함
	if( m_anWear[ pos ] == NULL )
		m_Inventory.AddWeightModifier( pItem->GetWeight() );

	UpdateWeightWithInventory();
	UpdateQuestStatusByItemUpgrade();
	// 장착 위치를 확인하지 않는다면 스왑용 장비에 의해 두 번 이상 장착되거나 장착 해제되는 경우가 발생할 수 있다.
	if( pos < ItemBase::MAX_ITEM_WEAR )
		UpdateTitleConditionByItemEquip( pItem->GetItemCode(), false );

	return nRet;
}

bool StructPlayer::IsEnemy( StructCreature* pTarget, bool bIncludeHiding )
{
	StructPlayer* pTargetPlayer = NULL;

	if( pTarget->IsSummon() )
	{
		pTargetPlayer = static_cast< StructSummon * >( pTarget )->GetMaster();
	}
	else if( pTarget->IsPlayer() )
	{
		pTargetPlayer = static_cast< StructPlayer * >( pTarget );
	}

	if( IsInSiegeDungeon() )
	{
		if( pTargetPlayer )
		{
			if( GetPartyID() && pTargetPlayer->GetPartyID() )
			{
				// 기존에는 GuildID로 했지만 길드 연합 시스템 추가시 문제가 발생하므로 LeadPartyID로 변경
				int lead_party_id = PartyManager::GetInstance().GetLinkedPartyLeadPartyID( pTargetPlayer->GetPartyID() );

				if( lead_party_id && lead_party_id != PartyManager::GetInstance().GetLinkedPartyLeadPartyID( GetPartyID() ) )
				{
					return true;
				}
			}
		}
		// 던전 시즈 안에서 던전 코어는 던전 공격자 길드에게만 적(주변 가디언이 모두 죽지 않았으면 적으로 판정 안 함)
		else if( pTarget->IsMonster() && static_cast< StructMonster * >( pTarget )->IsDungeonCore() )
		{
			int dungeon_id = DungeonManager::Instance().GetDungeonID( GetX(), GetY() );
			if( !dungeon_id )
			{
				return false;
			}

			int guild_id = GetGuildID();

			int alliance_id = GuildManager::GetInstance().GetAllianceID( guild_id );
			if( alliance_id )
			{
				guild_id = GuildManager::GetInstance().GetAllianceLeaderGuildID( alliance_id );
			}

#ifdef _DUNGEON_CORE_AS_MONSTER
			if( guild_id && guild_id == DungeonManager::Instance().GetRaidGuildID( dungeon_id ) )
			{
				// 코어 담당 가디언이 살아있는지 확인
				if( !DungeonManager::Instance().IsCoreInvincible( dungeon_id ) )
					return true;
			}
#endif

			return false;
		}
	}

	if( !StructCreature::IsEnemy( pTarget, bIncludeHiding ) )
		return false;

	if( pTargetPlayer )
	{
		if( IsInDeathmatch() && pTargetPlayer->IsInDeathmatch() && pTargetPlayer != this )
		{
			return true;
		}

		if( IsInBattleArena() )
		{
			if( BattleArenaManager::Instance().IsOpponent( this, pTargetPlayer ) )
				return true;
		}
	}

	if( pTarget->IsMonster() )
	{
		if( static_cast< StructMonster * >( pTarget )->IsEnvironmentMonster() )
			return false;

		// 대련장에서는 몹 못 잡아요~
		if( IsInBattleField() )
			return false;

		return true;
	}

	if( pTarget->IsNPC() && !IsInBattleField() && !IsInTown() )
	{
		if( IsPKOn() || IsBloodyCharacter() || IsDemoniacCharacter() )
			return true;
		else
			return false;
	}

	if( !pTargetPlayer )
		return false;

	if( pTargetPlayer == this )
		return false;

	if( pTargetPlayer->IsInvincible() || !pTargetPlayer->isMortal() )
		return false;

	if( GetCompeteID() == pTargetPlayer->GetCompeteID() && IsInStartedCompete( false ) )
		return true;

	if( IsInBattleField() )
	{
		return false;
	}
	else if( pTargetPlayer->IsInBattleField() )
	{
		return false;
	}

	if( !IsInPKField() )
	{
		return false;
	}

	if( !pTargetPlayer->IsInPKField() )
	{
		return false;
	}

	// 길드원은 적이 아니셈
	if( GetGuildID() && pTargetPlayer->GetGuildID() )
	{
		int nAllianceID = GuildManager::GetInstance().GetAllianceID( GetGuildID() );
		if( GetGuildID() == pTargetPlayer->GetGuildID() || ( nAllianceID && nAllianceID == GuildManager::GetInstance().GetAllianceID( pTargetPlayer->GetGuildID() ) ) )
			return false;
	}

	// 파티원도 적이 아니셈
	if( GetPartyID() && GetPartyID() == pTargetPlayer->GetPartyID() )
		return false;

	// 아무것도 아닌데 PK On이면 적이셈
	if( pTargetPlayer->IsPKOn() )
	{
		return true;
	}

	// PK Off 상태인데 블러디/데모니악이면 적이셈
	if( pTargetPlayer->IsBloodyCharacter() || pTargetPlayer->IsDemoniacCharacter() )
	{
		return true;
	}

	if( IsPKOffing() )
	{
		return false;
	}

	// 자신이 PK On 상태
	if( IsPKOn() )
	{
		return true;
	}

	return false;
}

const bool StructPlayer::TurnOnPkMode( const bool bImmediateOn )
{
	if( m_nTurnOffPkModeTime )
	{
		m_nTurnOffPkModeTime = 0;
		return true;
	}

	if( m_nTurnOnPkModeTime )
	{
		m_nTurnOnPkModeTime = 0;
		return false;
	}

	// 10초 후로 셋팅
	if( bImmediateOn )
		m_nTurnOnPkModeTime = GetArTime();
	else
		m_nTurnOnPkModeTime = GetArTime() + GameRule::PK_ON_TIME;

	return true;
}

const bool StructPlayer::TurnOffPkMode( const bool bImmediateOff )
{
	if( m_nTurnOnPkModeTime )
	{
		m_nTurnOnPkModeTime = 0;
		return true;
	}

	if( m_nTurnOffPkModeTime )
	{
		return false;
	}

	if( bImmediateOff )
		m_nTurnOffPkModeTime = GetArTime();
	else
		// 30초 후로 셋팅
		m_nTurnOffPkModeTime = GetArTime() + GameRule::PK_OFF_TIME;

	return true;
}

int StructPlayer::GetAllSkillTP() const
{
	if( GetJobDepth() < GameRule::MIN_TALENT_POINT_JOB_DEPTH )
		return 0;

	int nJobId = GetJobId();

	int nTP = 0;

	// 지금 가지고 있는 스킬들 배우는데 필요한 특성 포인트 계산
	for( std::vector< StructSkill * >::const_iterator it = m_vAllSkillList.begin() ; it != m_vAllSkillList.end() ; it++ )
	{
		StructSkill *pSkill = (*it);

		if( (*it)->GetSkillUID() == StructSkill::SKILL_UID_ITEM_SKILL ||
			(*it)->GetSkillUID() == StructSkill::SKILL_UID_PROP_SKILL ||
			(*it)->GetSkillUID() == StructSkill::SKILL_UID_SUMMON_SKILL ||
			(*it)->GetSkillUID() == StructSkill::SKILL_UID_MONSTER_SKILL ||
			(*it)->GetSkillUID() == StructSkill::SKILL_UID_PET_SKILL )
			continue;

		for( int nSkillLevel = pSkill->GetBaseSkillLevel() ; nSkillLevel > 0 ; nSkillLevel-- )
		{
			int tp = -1;

			tp = GameContent::GetNeedTpForSkillLevelUp( pSkill->GetSkillId(), nSkillLevel );

			if( tp != -1 )
			{
				nTP += tp;
			}
		}
	}

	return nTP;
}

int	StructPlayer::IsLearnableSkill( int nSkillID, int nSkillLevel, int *nSkillTreeID )
{
	int nResult = RESULT_ACCESS_DENIED;

	// 우선 전 직업에서 배울 수 있는 스킬 트리로 한번 검색해주고.
	for( int i = 0 ; i < 3 && ( nResult == RESULT_ACCESS_DENIED || nResult == RESULT_LIMIT_MAX || nResult == RESULT_NOT_ENOUGH_JOB_LEVEL ) ; ++i )
	{
		int nJobID = GetPrevJobId( i );
		if( nJobID )
		{
			const JobInfo *pInfo = GameContent::GetJobInfo( nJobID );
			if( !pInfo )
				continue;
			int nJobLevel = GetPrevJobLevel( i );

			nResult = GameContent::isLearnableSkill( this, nSkillID, nSkillLevel, pInfo->skill_tree_id, nJobLevel );
			if( nResult == RESULT_SUCCESS )
			{
				*nSkillTreeID = pInfo->skill_tree_id;
				break;
			}
		}
	}

	// 전 직업에서 못 배운다면 현재 직업에서 배울 수 있나 검색. (단 ERROR 조건이 밑 3개가 아니라면 검사할 필요가 없음.)
	if( nResult == RESULT_ACCESS_DENIED || nResult == RESULT_LIMIT_MAX || nResult == RESULT_NOT_ENOUGH_JOB_LEVEL )
	{
		const JobInfo *pInfo = GameContent::GetJobInfo( GetJobId() );
		if( !pInfo )
			return nResult;

		nResult = GameContent::isLearnableSkill( this, nSkillID, nSkillLevel, pInfo->skill_tree_id, GetJobLevel() );
		if( nResult == RESULT_SUCCESS )
		{
			*nSkillTreeID = pInfo->skill_tree_id;
		}
	}

	return nResult;
}

__int64 StructPlayer::GetDeadEXPPenalty()
{
	if( GameRule::bIsPKServer )
		return GameContent::GetNeedExp( GetLevel() ) * ( 0.3 / ( GetLevel() - 1 ) + 0.001 );

	return GameContent::GetNeedExp( GetLevel() ) * ( 0.15 / ( GetLevel() - 1 ) + 0.0005 );
}

void StructPlayer::procDecreaseEXPAndDropItem( StructPlayer * pKiller, StructItem ** pDropWearItem, StructItem ** pDropInvenItem )
{
	_DEAD_PENALTY_POINT dead_penalty( 1.0f, 0.0f );

	if( GameRule::bIsPKServer )
	{
		dead_penalty = _DEAD_PENALTY_POINT( 1.0f, 0.03f );
	}

	*pDropWearItem = NULL;
	*pDropInvenItem = NULL;

	if( IsInDeathmatch() )
	{
		SetLastDecreasedEXP( 0 );
		return;
	}

	if( IsInSiegeOrRaidDungeon() )
	{
		SetLastDecreasedEXP( 0 );
		return;
	}

	if( pKiller )
	{
		if( !GameRule::bIsPKServer )
		{
			SetLastDecreasedEXP( 0 );
			return;
		}
		else
		{
			dead_penalty = getDeadPenaltyPoint( IsPKOn(), GetImmoralPoint(), pKiller->GetLevel() - GetLevel() );
		}
	}


	if( GetLevel() <= 5 )
	{
		SetLastDecreasedEXP( 0 );
		return;
	}

	if( GameRule::bIsPKServer )
	{
		int nLP = int( pow( GetLevel() - 5, 1.44f ) );

		if( GetImmoralPoint() < GameRule::MORAL_LIMIT )
		{
			nLP /= 2;

			if( pKiller && pKiller->GetLevel() - GetLevel() >= GameRule::nPKPenaltyLevel )
			{
				nLP /= 2;
			}
		}

		// 라크 가드 발동
		if( GetChaos() >= nLP )
		{
			dead_penalty.fEP = 0;
			dead_penalty.fDP /= 2.0f;

			AddChaos( 0 - nLP );
			PrintfChatMessage( false, CHAT_EXP, "@COMMON", this, "@523\v#@num@#\v%d", nLP );

			if( pKiller )
			{
				TS_SC_GET_CHAOS msg;

				msg.hPlayer = pKiller->GetHandle();
				msg.hCorpse = GetHandle();
				msg.nChaos = nLP;
				msg.nBonusType = TS_SC_GET_CHAOS::CHAOS_BONUS_NONE;
				msg.nBonusPercent = 0;
				msg.nBonus = 0;

				ArcadiaServer::Instance().Broadcast( GetRX(), GetRY(), GetLayer(), &msg );

				pKiller->AddChaos( nLP * 0.75f );
			}
		}
	}

	__int64 nLostExp = GetDeadEXPPenalty();

	nLostExp *= dead_penalty.fEP;

	if( GetImmoralPoint() > GameRule::CRIME_LIMIT )
	{
		nLostExp *= 3;
	}
	else if( GetImmoralPoint() > GameRule::MORAL_LIMIT )
	{
		nLostExp *= ( 1 + int( GetImmoralPoint() / 100 ) * 0.2f );
	}

	// hardcore server
	if(GameRule::Hardcore)
	{
		if(GetState(static_cast<StructState::StateCode>(2016014)))
		{
			nLostExp = 0;
		}
		else
		{
			nLostExp *= GameRule::HardcoreExpRate;
		}
		if(IsPKOn())
		{
			// 경험치, 아이템 상실 증가2
		}
		else
		{
			// 경험치, 아이템 상실 감소
		}
	}

	SetLastDecreasedEXP( nLostExp );

	SetEXP( GetEXP() - nLostExp );

	// 요정의 자물쇠 처리
	StructItem * pFairyLock = FindItem( static_cast< ItemBase::ItemCode >( ItemBase::ITEM_CODE_FAIRY_LOCK ) );

	StructItem * pItem = NULL;

	if( !pFairyLock && XRandom( 1, 999 ) < (int)( dead_penalty.fDP * 1000 ) )
	{
		// 아이템 드랍

		int pos = XRandom( 0, ItemBase::MAX_SPARE_ITEM_WEAR - 1 );

		for( int i = 0; i < ItemBase::MAX_SPARE_ITEM_WEAR; ++i )
		{
			if( m_anWear[pos] )
			{
				pItem = m_anWear[pos];

				while( true )
				{
					if( pItem->IsCashItem() )
					{
						pItem = NULL;
						break;
					}

					if( pItem->IsIdentified() )
					{
						// 랜덤화 옵션이 붙은 아이템들은 계정 귀속이라 안 떨어짐
						pItem = NULL;
						break;
					}

					if( Putoff( static_cast< ItemBase::ItemWearType >( pos ) ) != RESULT_SUCCESS )
					{
						pItem = NULL;
						break;
					}

					if( !DropItem( pItem, pItem->GetCount() ) )
					{
						pItem = NULL;
						break;
					}

					// 벗은거 방송
					if( pos < ItemBase::MAX_ITEM_WEAR )
					{
						TS_WEAR_INFO msg;
						GetWearMsg( this, msg );
						ArcadiaServer::Instance().Broadcast( GetRX(), GetRY(), GetLayer(), &msg );
					}

					break;
				}

				if( pItem )
					break;
			}

			++pos;

			if( pos >= ItemBase::MAX_SPARE_ITEM_WEAR )
			{
				pos = 0;
			}
		}
	}

	*pDropWearItem = pItem;

	pItem = NULL;

	if( !pFairyLock && XRandom( 1, 999 ) < (int)( dead_penalty.fDP * 1000 ) )
	{
		// 아이템 드랍

		size_t cnt = m_Inventory.GetCount();

		size_t pos = ( cnt > 0 ) ? XRandom( 0, static_cast<unsigned int>( cnt ) - 1 ) : 0;

		for( size_t i = 0 ; i < cnt ; ++i, pItem = NULL, pos = ++pos % cnt )
		{
			pItem = m_Inventory.Get( pos );

			// 소환수 카드는 기획으로는 안떨어져야 하지만 지금까지 떨어졌어서
			// 떨어지지 않도록 잠수함 패치. 불필요한 운영이슈 발생 방지를 위하여~
			if( pItem->IsCube() || pItem->IsCashItem() || ( pItem->IsSummonCard() && pItem->GetInstanceFlag().IsOn( ItemInstance::ITEM_FLAG_SUMMON ) ) )
			{
				continue;
			}

			// 장착되어 있는 아이템일 경우 처리(소환수 장착 장비 또는 스킬 카드)
			if( pItem->GetBindedCreatureHandle() )
			{
				StructCreature::iterator it = StructCreature::get( pItem->GetBindedCreatureHandle() );
				StructCreature *pCreature = (*it);

				if( !pCreature )
				{
					continue;
				}

				// 장착된 아이템인 경우
				if( pItem->GetWearInfo() != ItemBase::WEAR_NONE )
				{
					// 소환수가 장착하고 있는 아이템일 경우 소환수가 시야 거리 내에 있는지 체크(거리 밖이면 지역 락이 걸려있지 않음)
					// 캐릭터가 장착하고 있는 아이템의 경우도 여기서 걸려서 다음 것으로 넘어감
					if( !pCreature->IsSummon() || ( pCreature->IsInWorld() && pCreature->GetPos().GetDistance( GetPos() ) >= GameRule::VISIBLE_RANGE ) )
					{
						continue;
					}

					// 장착 해제가 불가능하면 다음 아이템으로 넘어감
					if( pCreature->Putoff( pItem->GetWearType() ) != RESULT_SUCCESS )
					{
						continue;
					}
				}

				// 장착되어있는 스킬 카드라면 장착 해제
				if( pItem->IsSkillCard() )
				{
					UnBindSkillCard( pItem );
				}
			}

			// 드랍 성공했으면 루프 탈출
			if( pItem = DropItem( pItem, pItem->GetCount() ) )
			{
				break;
			}
		}

	}

	*pDropInvenItem = pItem;

	// 요정의 자물쇠 사용됐으면 제거 처리
	if( pFairyLock )
	{
		SendChatMessage( false, CHAT_ITEM, "@SYSTEM", this, "@241" );

		LOG::Log11N4S( LM_ITEM_USE, GetAccountID(), GetSID(), pFairyLock->GetItemEnhance() * 100 + pFairyLock->GetItemLevel(), pFairyLock->GetItemCode(), 1, pFairyLock->GetCount() - 1, 0, 0, 0, 0, pFairyLock->GetItemUID(), GetAccountName(), LOG::STR_NTS, GetName(), LOG::STR_NTS, "", 0, "", 0 );

		EraseItem( pFairyLock, 1 );

		Save();
	}
}

void StructPlayer::ProcImmoralPoint( StructPlayer * pKiller, int nChaos, bool bDecreaseProc )
{
	if( IsInSiegeOrRaidDungeon() )
		return;

	// PK한 사람 부도덕 수치 증가
	if( pKiller )
	{
		float fIPInc = 0.0f;
		char * szPkType = "PKNM";

		if( IsPKOn() )
		{
			szPkType = "PKPK";
		}
		else if( IsBloodyCharacter() || IsDemoniacCharacter() )
		{
			szPkType = "PKBD";
		}
		else
		{
			float fPenalty = 1.0f;

			if( pKiller->GetLevel() - GetLevel() >= GameRule::nPKPenaltyLevel )
			{
				fIPInc = 100.0f;

				// 모럴 포인트별 PK 페널티 적용( 1이상 : 모럴 감소 +200 / 1000이상 : 모럴 감소 +1000 )
				if( pKiller->GetImmoralPoint() <= -1000.0f )
					fIPInc = 1000.0f;
				else if( pKiller->GetImmoralPoint() < 0.0f )
					fIPInc = 200.0f;

				pKiller->IncDKC();

				szPkType = "PKDK";
			}
			else
			{
				fIPInc = 50.0f;
			}


			if( pKiller->GetImmoralPoint() > 1000 )
			{
				fPenalty += 1.0f;
			}
			else if( pKiller->GetImmoralPoint() > 0 )
			{
				fPenalty += ( (int) (pKiller->GetImmoralPoint() * 0.01f ) ) * 0.1f;
			}

			if( pKiller->GetDKC() >= 20 )
			{
				fPenalty += 1.0f;
			}
			else if( pKiller->GetDKC() >= 5 )
			{
				fPenalty += 0.5f;
			}

			fIPInc *= fPenalty;

			if( GameRule::bIsPKServer && pKiller->GetPartyID() )
			{
				int nCount = PartyManager::GetInstance().GetMemberCount( pKiller->GetPartyID() );

				fIPInc *= 2 - 2 / ( nCount + 1 );
			}

			bool bIsNeedToSendStatus = false;

			if( pKiller->GetImmoralPoint() < GameRule::MORAL_LIMIT && ( pKiller->GetImmoralPoint() + fIPInc ) >= GameRule::MORAL_LIMIT )
			{
				bIsNeedToSendStatus = true;
			}
			else if( pKiller->GetImmoralPoint() < GameRule::CRIME_LIMIT && ( pKiller->GetImmoralPoint() + fIPInc ) >= GameRule::CRIME_LIMIT )
			{
				bIsNeedToSendStatus = true;
			}

			pKiller->SetImmoralPoint( pKiller->GetImmoralPoint() + fIPInc );

			pKiller->IncPKC();
			pKiller->m_QuestManager.UpdateQuestStatusByPlayerKill( this );

			if( bIsNeedToSendStatus )
			{
				BroadcastStatusMessage( pKiller );
			}

			AR_TIME t = GetArTime();

			if( !GameRule::bIsPKServer )
			{
				// 천벌 걸자~ (여기서 거는 건 좀 우울하긴 하다만. 뭐 적절한 위치이기도 하니.)
				if( pKiller->GetImmoralPoint() >= GameRule::CRIME_LIMIT )
				{
					pKiller->AddState( StructState::NEMESIS, 0, 3, t, t + 2880000 );
				}
				else if( pKiller->GetImmoralPoint() >= GameRule::SEMI_CRIME_LIMIT )
				{
					pKiller->AddState( StructState::NEMESIS, 0, 2, t, t + 1440000 );
				}
				else if( pKiller->GetImmoralPoint() >= GameRule::MORAL_LIMIT )
				{
					pKiller->AddState( StructState::NEMESIS, 0, 1, t, t + 720000 );
				}
			}
		}

		pKiller->m_TitleManager.UpdateTitleConditionByPlayerKill( GetImmoralPoint() );

		LOG::Log11N4S( LM_CHARACTER_PLAYER_KILL, pKiller->GetAccountID(), pKiller->GetSID(), 0, GetSID(), nChaos, fIPInc, pKiller->GetImmoralPoint(), pKiller->GetDKC(), pKiller->GetPKC(), pKiller->GetChaos(), 0, pKiller->GetAccountName(), LOG::STR_NTS, pKiller->GetName(), LOG::STR_NTS, GetName(), LOG::STR_NTS, szPkType, LOG::STR_NTS );
	}

	if( ( !pKiller || GameRule::bIsPKServer ) && bDecreaseProc && GetImmoralPoint() > 0 )
	{
		float fIPDec = -10.0f;

		if( m_nPKC < 100 )
		{
			fIPDec += (int) ( m_nPKC * 0.1f );
		}
		else
		{
			fIPDec = -1;
		}

		bool bIsNeedToSendStatus = false;

		if( GetImmoralPoint() >= GameRule::CRIME_LIMIT && ( GetImmoralPoint() + fIPDec ) < GameRule::CRIME_LIMIT )
		{
			bIsNeedToSendStatus = true;
		}
		else if( GetImmoralPoint() >= GameRule::MORAL_LIMIT && ( GetImmoralPoint() + fIPDec ) < GameRule::MORAL_LIMIT )
		{
			bIsNeedToSendStatus = true;
		}

		SetImmoralPoint( GetImmoralPoint() + fIPDec );

		if( GetImmoralPoint() <= 0 )
		{
			SetImmoralPoint( 0 );
			bIsNeedToSendStatus = true;
		}

		if( bIsNeedToSendStatus )
			BroadcastStatusMessage( this );

	}
	// }
}

bool StructPlayer::IsAlly( StructCreature* pTarget )
{
	if( pTarget->IsMonster() )
	{
		StructMonster * pMonster = static_cast< StructMonster * >( pTarget );

		if( pMonster->IsOriginalDungeonOwnerGuardian() || pMonster->IsOriginalDungeonSiegerGuardian() )
		{
			int dungeon_id = DungeonManager::Instance().GetDungeonID( pMonster->GetX(), pMonster->GetY() );

			if( dungeon_id )
			{
				int guild_id = PartyManager::GetInstance().GetAttackTeamGuildID( GetPartyID() );

				if( guild_id == DungeonManager::Instance().GetOriginalOwnGuildID( dungeon_id ) )
				{
					if( pMonster->IsOriginalDungeonOwnerGuardian() )
						return true;
				}
				else
				{
					if( pMonster->IsOriginalDungeonSiegerGuardian() )
						return true;
				}

				if( guild_id == DungeonManager::Instance().GetOwnGuildID( dungeon_id ) && pMonster->IsDungeonCore() )
					return true;
			}
		}
	}

	StructPlayer * pTargetPlayer = NULL;
	if( pTarget->IsSummon() )
	{
		pTargetPlayer = static_cast< StructSummon * > (pTarget)->GetMaster();

		if( !pTargetPlayer )
			return false;
	}
	else if( pTarget->IsPlayer() )
	{
		pTargetPlayer = static_cast< StructPlayer * >( pTarget );
	}

	if( pTargetPlayer )
	{
		if( pTargetPlayer == this )
			return true;
		if( GetCompeteID() == pTargetPlayer->GetCompeteID() && IsInStartedCompete( false ) )
			return false;
		if( IsInDeathmatch() && pTargetPlayer->IsInDeathmatch() )
			return false;
	}

	if( GetPartyID() )
	{
		if( pTargetPlayer )
		{
			if( GetPartyID() == pTargetPlayer->GetPartyID() )
			{
				return true;
			}

			int guild_id = PartyManager::GetInstance().GetAttackTeamGuildID( pTargetPlayer->GetPartyID() );
			if( guild_id && guild_id == PartyManager::GetInstance().GetAttackTeamGuildID( GetPartyID() ) )
			{
				return true;
			}

			int nLeadPartyID = PartyManager::GetInstance().GetLinkedPartyLeadPartyID( GetPartyID() );
			if( nLeadPartyID && nLeadPartyID == PartyManager::GetInstance().GetLinkedPartyLeadPartyID( pTargetPlayer->GetPartyID() ) )
			{
				return true;
			}
		}
	}
	else
	{
		if( pTarget->IsSummon() && static_cast< StructSummon * >( pTarget )->GetMaster() == this )
			return true;
		else if( pTarget == this )
			return true;
	}

	return StructCreature::IsAlly( pTarget );
}

bool StructPlayer::IsVisible( StructCreature *pTarget )
{
	return StructCreature::IsVisible( pTarget ) || ( m_pMainSummon && m_pMainSummon->StructCreature::IsVisible( pTarget ) ) || ( m_pSubSummon && m_pSubSummon->StructCreature::IsVisible( pTarget ) );
}

void StructPlayer::SetLastContact( const char * szKey, const char * szValue )
{
	if( m_hsContact.has( szKey ) )
	{
		m_hsContact.modify( szKey, szValue );
	}
	else
	{
		m_hsContact.add( szKey, szValue );
	}
}

void StructPlayer::SetLastContact( const char * szKey, int nValue )
{
	std::string szValue;
	XStringUtil::Format( szValue, "%d", nValue );

	SetLastContact( szKey, szValue.c_str() );
}

std::string StructPlayer::GetLastContactStr( const char * szKey ) const
{
	std::string strResult;
	m_hsContact.lookup( szKey, strResult );
	return strResult;
}

int StructPlayer::GetLastContactLong( const char * szKey ) const
{
	std::string szValue = GetLastContactStr( szKey );

	return atoi( szValue.c_str() );
}

void StructPlayer::SetContinuousPlayTime( AR_TIME continuous_play_time )
{
	AR_TIME nContinuousPlayTime = std::min( continuous_play_time, GameRule::nMaxTiredGameTime );

	// 스테미너 세이버 관련 처리
	StructState * pStaminaSave = GetState( StructState::STAMINA_SAVE );

	if( pStaminaSave )
	{
		if( pStaminaSave->IsHolded() )
		{
			if( nContinuousPlayTime < GameRule::nMaxHealthyGameTime && ( GetPCBangMode() != GameRule::PCBANG_PREMIUM_BONUS || GameRule::bApplyStaminaBonusInPremiumPCBang ) )
			{
				pStaminaSave->ReleaseRemainDuration();
				onUpdateState( *pStaminaSave );
			}
		}
		else
		{
			if( nContinuousPlayTime >= GameRule::nMaxHealthyGameTime )
			{
				pStaminaSave->HoldRemainDuration();
				onUpdateState( *pStaminaSave );
			}
		}
	}

	// 성장의 물약류 처리
	static const StructState::StateCode SUPER_SAVE_CODE_LIST[] = { StructState::SUPER_SAVE_0, StructState::SUPER_SAVE_1, StructState::SUPER_SAVE_2, StructState::SUPER_SAVE_3 };
	for( int i = 0 ; i < _countof( SUPER_SAVE_CODE_LIST ) ; ++i )
	{
		StructState * pSuperSave = GetState( SUPER_SAVE_CODE_LIST[ i ] );
		if( pSuperSave )
		{
			if( pSuperSave->IsHolded() )
			{
				if( nContinuousPlayTime < GameRule::nMaxHealthyGameTime )
				{
					pSuperSave->ReleaseRemainDuration();
					onUpdateState( *pSuperSave );
				}
			}
			else
			{
				if( nContinuousPlayTime >= GameRule::nMaxHealthyGameTime )
				{
					pSuperSave->HoldRemainDuration();
					onUpdateState( *pSuperSave );
				}
			}
		}
	}

	m_nContinuousPlayTime = nContinuousPlayTime;

	SendPropertyMessage( this, GetHandle(), "playtime", GetContinuousPlayTime() );
}

bool StructPlayer::Summon( struct StructSummon * pSummon, const ArPosition & pos )
{
	if( !IsSummonable() )
	{
		return false;
	}

	if( pSummon->IsInWorld() )
		return false;

	if( GetSubSummon() && GetMainSummon() )
	{
		// 두번째 소환수 들어가야 소환 가능.
		return false;
	}

	if( pSummon->IsInvincible() )
		pSummon->SetInvincible( false );

	if( GetMainSummon() )
	{
		// 우선 보조 소환수로 설정
		m_pSubSummon = pSummon;

		// 편성 순서를 기준으로 주/부 소환수 결정
		SetMainAndSubSummon();

		// 최종적으로 결정된 보조 소환수의 역소환 카운트 방송
		AR_TIME t = GetArTime();
		AR_TIME nNextUnSummonTime = m_bIsInfiniteSummonTime ? 0 : m_nDoubleSummonTime;

		TS_SC_UNSUMMON_NOTICE msg;

		msg.summon_handle = m_pSubSummon ? m_pSubSummon->GetHandle() : 0;
		msg.unsummon_duration = nNextUnSummonTime;

		PendMessage( this, &msg );

		SetNextUnSummonTime( t + nNextUnSummonTime );
	}
	else
	{
		m_pMainSummon = pSummon;
	}

	// 소환
	pSummon->SetCurrentXY( pos.x, pos.y );
	pSummon->SetCurrentLayer( GetLayer() );
	pSummon->StopMove();

	if( pSummon->IsDead() )
	{
		pSummon->SetDeadTime( GetArTime() );
	}

	AddSummonToWorld( pSummon );
	ArcadiaServer::Instance().SetObjectPriority( pSummon, ArSchedulerObject::UPDATE_PRIORITY_NORMAL );

	if( IsInDeathmatch() )
	{
		pSummon->RemoveStateByEnteringDeathmatch();
	}

	// 역소환 상태 중에 걸려있던 지속효과가 없어졌거나 스텟 변화가 생기면 소환 후에 재계산 해줘야 함
	pSummon->SetNeedCalculateStat();

	checkSummonEffectOnPlayer();

	BroadcastSummonInfo( GetPartyID(), this );

	return true;
}

bool StructPlayer::UnSummon( struct StructSummon * pSummon )
{
	StructSummon * pUnSummonTarget = NULL;

	if( pSummon )
	{
		pUnSummonTarget = pSummon;
	}
	else if( GetSubSummon() )
	{
		pUnSummonTarget = GetSubSummon();

	}
	else if( GetMainSummon() )
	{
		pUnSummonTarget = GetMainSummon();
	}
	else
	{
		return false;
	}

	if( !pUnSummonTarget )
		return false;

	bool returnValue = unSummon( pUnSummonTarget );

	checkSummonEffectOnPlayer();

	BroadcastSummonInfo( GetPartyID(), this, true );

	return returnValue;
}

void StructPlayer::checkSummonEffectOnPlayer()
{
	for ( unsigned int i = 0; i < m_vPassiveSkillList.size(); i++)
	{
		if ( m_vPassiveSkillList[i]->GetSkillBase()->GetSkillEffectType() == SkillBase::EF_INC_PARAM_BASED_SUMMON_PARAM )
		{
			SetNeedCalculateStat();
		}
	}
}

bool StructPlayer::unSummon( StructSummon * pSummon )
{
	if( pSummon->IsInvincible() )
	{
		pSummon->SetInvincible( false );
	}

	if( pSummon->IsInWorld() )
	{
		if( GetRideObject() == pSummon )
		{
			UnMount( UNMOUNT_UNSUMMON );
		}

		if( pSummon == GetSubSummon() )
		{
			m_pSubSummon = NULL;
			m_nNextUnSummonTime = 0;
		}
		else if( pSummon == GetMainSummon() )
		{
			m_pMainSummon = NULL;

			if( GetSubSummon() )
			{
				m_pMainSummon = GetSubSummon();
				if( m_nRideIdx == MOUNT_ON_SUB )
				{
					m_nRideIdx = MOUNT_ON_MAIN;
				}
				m_pSubSummon = NULL;
				m_nNextUnSummonTime = 0;
			}
		}

		if( pSummon->IsMoving() )
		{
			// 안하면 이동 중에 역소환 됐을 때, 소환수가 월드에 없는데도 IsMoving() 함수가 true를 반환함
			ArPosition pos = pSummon->GetCurrentPosition( GetArTime() );
			pSummon->SetMove( pos, 0 );
			pSummon->StopMove();
		}

		if( pSummon->IsUsingSkill() )
		{
			pSummon->CancelSkill();
		}

		pSummon->RemoveAllAura();
		pSummon->RemoveAllStateByDeadOrLogout();

		pSummon->RemoveAllHate();

		pSummon->DBQuery( new DB_UpdateSummon( pSummon ) );

		TS_SC_UNSUMMON msg;
		msg.summon_handle = pSummon->GetHandle();

		ArcadiaServer::Instance().Broadcast( pSummon->GetRX(), pSummon->GetRY(), pSummon->GetLayer(), &msg );

		if( !IsVisibleRegion( pSummon->GetRX(), pSummon->GetRY(), GetRX(), GetRY() ) )
		{
			PendMessage( this, &msg );
		}

		pSummon->SetSummonFlag( false );

		// 역소환
		RemoveSummonFromWorld( pSummon );
	}
	return true;
}

void StructPlayer::procPendingUnSummon()
{
	// 여기서 interlocked_exchange를 쓰고 m_pMainSummon, m_pSubSummon이 역소환 처리되기 위한 지역락을 걸려다 대기 상태가 되면
	// 역소환 대기 상태 플래그는 0으로 초기화되어 있지만 m_pMainSummon, m_pSubSummon이 아직 세팅되어 있고 월드에도 있으므로
	// 다른 쓰레드에서 보기에 역소환 대기 상태가 아닌 것으로 보여질 수 있음(StructPlayer::IsErasable 에서 소환수 카드 관련 assert 걸릴 수 있음)
	int nUnSummonIndex = GetPendingUnSummon();
	while( nUnSummonIndex )
	{
		StructSummon *pSummon = NULL;

		if( nUnSummonIndex & UNSUMMON_SUB )
		{
			pSummon = GetSubSummon();
			nUnSummonIndex ^= UNSUMMON_SUB;
		}
		else if( nUnSummonIndex & UNSUMMON_MAIN )
		{
			pSummon = GetMainSummon();
			nUnSummonIndex ^= UNSUMMON_MAIN;
		}

		if( pSummon && pSummon->IsInWorld() )
		{
			ARCADIA_LOCK( ArcadiaServer::Instance().LockObjects( this, pSummon ) );

			UnSummon( pSummon );
		}
	}
	InterlockedExchange( &m_nPendingUnSummon, 0 );
}

bool StructPlayer::PendUnSummon( struct StructSummon * pSummon )
{
	if( pSummon && pSummon->IsInWorld() )
	{
		if( pSummon == GetMainSummon() )
		{
			InterlockedExchange( &m_nPendingUnSummon, m_nPendingUnSummon | UNSUMMON_MAIN );
			return true;
		}
		else if( pSummon == GetSubSummon() )
		{
			InterlockedExchange( &m_nPendingUnSummon, m_nPendingUnSummon | UNSUMMON_SUB );
			return true;
		}
	}

	return false;
}

void StructPlayer::SetMainAndSubSummon()
{
	// 보조 소환수가 없다면 변경될 여지가 없다.
	if( !m_pMainSummon || !m_pSubSummon )
		return;

	int nCreatureControlLevel = GetCurrentPassiveSkillLevel( StructSkill::SKILL_CREATURE_CONTROL );
	if( nCreatureControlLevel > 6 )
		nCreatureControlLevel = 6;

	StructSummon *pMainSummon = NULL;
	StructSummon *pSubSummon = NULL;

	for( int i = 0; i < nCreatureControlLevel - 1; ++i )
	{
		StructSummon *pSummon = GetSummonAt( i );

		// 편성 번호가 낮은 소환수가 주 소환수가 된다
		if( pSummon == m_pMainSummon )
		{
			pMainSummon = m_pMainSummon;
			pSubSummon = m_pSubSummon;

			break;
		}
		else if( pSummon == m_pSubSummon )
		{
			pMainSummon = m_pSubSummon;
			pSubSummon = m_pMainSummon;

			break;
		}
	}

	// 소환수를 찾지 못 한 경우는 문제가 발생할 수 있다.
	assert( pMainSummon && pSubSummon );

	// 주 소환수가 바뀌었다면 그에 따른 처리를 해준다
	if( pMainSummon != m_pMainSummon )
	{
		m_pMainSummon = pMainSummon;
		m_pSubSummon = pSubSummon;

		// 역소환 대기 상태 교환
		if( m_nPendingUnSummon & UNSUMMON_MAIN )
			InterlockedExchange( &m_nPendingUnSummon, UNSUMMON_SUB );
		else if( m_nPendingUnSummon & UNSUMMON_SUB )
			InterlockedExchange( &m_nPendingUnSummon, UNSUMMON_MAIN );

		BroadcastSummonInfo( GetPartyID(), this );
	}
}

StructItem * StructPlayer::GetEquipmentOnBelt() const
{
	for( int i = 0; i < m_nBeltSlotMax; ++i )
	{
		StructItem *pItem = m_aBeltSlotCard[i];
		
		if( !pItem || !pItem->IsEquipmentOnBelt() )
			continue;

		return pItem;
	}

	return NULL;
}

void StructPlayer::setSummonUpdate()
{
	std::vector< StructSummon * >::iterator it;

	for( it = m_vSummonList.begin(); it != m_vSummonList.end(); ++it )
	{
		if( (*it)->IsInWorld() )
			(*it)->SetNeedCalculateStat();
		else
			(*it)->CalculateStat();

	}
}

bool StructPlayer::FarmSummon( int nSlot, AR_HANDLE hCardHandle, int nDuration, bool bUseCracker, bool bCash )
{
	StructItem *pCard = StructItem::FindItem( hCardHandle );
	if( !pCard || !pCard->IsSummonCard() ) return false;
	if( GetFarmedSummonInfo( nSlot ) ) return false;
	if( GetFarmedSummonCount() >= GameRule::FARM_MAX_COUNT ) return false;
	if( !bCash && GetNonCashFarmedSummonCount() >= GameRule::FARM_NON_CASH_MAX_COUNT ) return false;

	// 봉인된 소환수 카드이면서 아직 농장에 맡기지 않은 아이템
	// 기간제이거나 내구도가 다 한 소환수는 맡길 수 없음
	if(	!pCard->IsSummonCard() || !pCard->GetInstanceFlag().IsOn( ItemInstance::ITEM_FLAG_SUMMON ) || pCard->GetInstanceFlag().IsOn( ItemInstance::ITEM_FLAG_FARMED_SUMMON ) || pCard->IsExpireItem() || ( !pCard->GetCurrentEtherealDurability() && pCard->GetMaxEtherealDurability() ) )
		return false;

	// 한 번도 편성되지 않음
	StructSummon *pSummon = pCard->GetSummonStruct();
	if( !pSummon ) return false;

	// 레벨 및 진화 정도
	int nTransformLevel = pSummon->GetTransformLevel();
	if(	pSummon->GetLevel() >= GameRule::FARM_MAX_LEVEL
		|| ( nTransformLevel == SummonBase::EVOLVE_NORMAL && pSummon->GetLevel() >= GameRule::NORMAL_SUMMON_MAX_LEVEL )
		|| ( nTransformLevel == SummonBase::EVOLVE_GROWTH && pSummon->GetLevel() >= GameRule::GROWTH_SUMMON_MAX_LEVEL ) 
		|| ( nTransformLevel == SummonBase::EVOLVE_EVOLVE && pSummon->GetLevel() >= GameRule::EVOLVE_SUMMON_MAX_LEVEL )	)
		return false;

	// 프리미엄 농장 이용권이 아닐 경우 나보다 레벨 더 높으면 못 맡김
	if( !bCash && pSummon->GetLevel() >= GetLevel() )
		return false;

	// 소환된 소환수
	if( !pSummon || pSummon->IsInWorld() ) return false;

	// 편성되어 있거나 벨트 장착 중인 소환수 카드
	for( int nSummonIdx = 0 ; nSummonIdx < 6 ; ++nSummonIdx )
		if( GetSummonCardAt( nSummonIdx ) == pCard || GetBeltSlotCardAt( nSummonIdx ) == pCard )
			return false;

	int nFarmUID = StructPlayer::AllocFarmSID();

	StructItem *pFarmedCard = PopItem( pCard, pCard->GetCount(), true );
	// 인벤에서 아이템 꺼내는데 실패(소유권 정보 이상 등)
	if( !pFarmedCard )
	{
		assert( 0 );
		return false;
	}

	// 아이템 정보 및 목록 업데이트
	// 인벤토리에서는 빠지지만 소유권은 변하지 않아야 한다
	pCard->SetIdx( 0 );
	pCard->SetInstanceFlagOn( ItemInstance::ITEM_FLAG_FARMED_SUMMON );
	pCard->SetOwnerInfo( GetHandle(), GetSID(), 0 );
	pSummon->SetMaster( this );

	time_t currentTime = time( NULL );
	SetFarmedSummonInfo( nSlot, new FARMED_SUMMON_INFO( nFarmUID, pCard, bCash? GameRule::FARM_MAX_LEVEL : GetLevel(), bUseCracker, bCash, currentTime, nDuration, 0 ) );

	pCard->DBQuery( new DB_UpdateItem( pCard ) );
	pSummon->DBQuery( new DB_UpdateSummon( pSummon ) );
	DBQuery( new DB_InsertFarmInfo( this, nFarmUID, nSlot + 1, pCard, bCash? GameRule::FARM_MAX_LEVEL : GetLevel(), bUseCracker, bCash, currentTime, nDuration, 0 ) );

	char buf[12];
	s_sprintf( buf, _countof( buf ), "%d", nDuration );

	LOG::Log11N4S( LM_FARM_SUMMON, GetAccountID(), GetSID(), pCard->GetItemCode(), nFarmUID, pSummon->GetSID(), pSummon->GetSummonCode(), pSummon->GetLevel(), pSummon->GetEXP(), bUseCracker, bCash, pCard->GetItemUID(), GetAccountName(), LOG::STR_NTS, GetName(), LOG::STR_NTS, pSummon->GetName(), LOG::STR_NTS, buf, LOG::STR_NTS );
	return true;
}

bool StructPlayer::RegainSummon( AR_HANDLE hHandle )
{
	StructItem *pCard = StructItem::FindItem( hHandle );
	if( !pCard ) return false;

	// 맡겨지지 않은 카드
	if( !pCard->GetInstanceFlag().IsOn( ItemInstance::ITEM_FLAG_FARMED_SUMMON ) )
	{
		return false;
	}

	for( int i = 0; i < GameRule::FARM_MAX_COUNT; ++i )
	{
		FARMED_SUMMON_INFO *pInfo = m_vFarmedSummonInfo[i];
		if( !pInfo ) continue;

		if( pInfo->item->GetItemUID() == pCard->GetItemUID() )
		{
			StructSummon *pSummon = pCard->GetSummonStruct();
			if( !pSummon )
			{
				assert( 0 );
				return false;
			}

			// 소유자가 자신인 경우 PushItem이 수행되지 않는다
			pCard->SetOwnerInfo( 0, 0, 0 );
			pSummon->SetMaster( NULL );
			pCard->SetInstanceFlagOff( ItemInstance::ITEM_FLAG_FARMED_SUMMON );

			PushItem( pCard, pCard->GetCount() );

			pCard->DBQuery( new DB_UpdateItem( pCard ) );
			SendItemMessage( this, pCard );

			time_t tHour = ( std::min( time( NULL ), pInfo->registration_time + pInfo->duration ) - pInfo->registration_time ) / 3600;
			double dRate = pInfo->is_using_cracker ? 1.5 : 1.0;
			__int64 nExp = 0;
			__int64 nExpLimit = 0;
			int nLevelLimit = std::min( pInfo->max_level, GameRule::FARM_MAX_LEVEL );

			switch( pSummon->GetTransformLevel() )
			{
			case SummonBase::EVOLVE_NORMAL:
				if( pInfo->is_cash )
					nExp = tHour * GameRule::nPremiumFarmNormalSummonEXP * dRate;
				else
					nExp = tHour * GameRule::nFarmNormalSummonEXP * dRate;
				nLevelLimit = std::min( nLevelLimit, GameRule::NORMAL_SUMMON_MAX_LEVEL );
				break;
			case SummonBase::EVOLVE_GROWTH:
				if( pInfo->is_cash )
					nExp = tHour * GameRule::nPremiumFarmGrowthSummonEXP * dRate;
				else
					nExp = tHour * GameRule::nFarmGrowthSummonEXP * dRate;
				nLevelLimit = std::min( nLevelLimit, GameRule::GROWTH_SUMMON_MAX_LEVEL );
				break;
			case SummonBase::EVOLVE_EVOLVE:
				if( pInfo->is_cash )
					nExp = tHour * GameRule::nPremiumFarmEvolveSummonEXP * dRate;
				else
					nExp = tHour * GameRule::nFarmEvolveSummonEXP * dRate;
				nLevelLimit = std::min( nLevelLimit, GameRule::EVOLVE_SUMMON_MAX_LEVEL );
				break;
			default:
				return false;
			}

			int nPreviousLevel = pSummon->GetLevel();
			__int64 nPreviousExp = pSummon->GetEXP();
			nExpLimit = GameContent::GetNeedSummonExp( nLevelLimit ) - 1;

			if( nExpLimit > pSummon->GetEXP() )
			{
				nExp = std::min( nExp, nExpLimit - pSummon->GetEXP() );
				pSummon->AddExp( nExp, 0, false, true );
			}

			LOG::Log11N4S( LM_FARM_REGAIN, GetAccountID(), GetSID(), pCard->GetItemCode(), pInfo->farm_sid, pSummon->GetSID(), pSummon->GetSummonCode(), nPreviousLevel, nPreviousExp, pSummon->GetLevel(), pSummon->GetEXP(), pCard->GetItemUID(), GetAccountName(), LOG::STR_NTS, GetName(), LOG::STR_NTS, pSummon->GetName(), LOG::STR_NTS, "", 0 );

			DBQuery( new DB_DeleteFarmInfo( this, pInfo->farm_sid ) );
			delete pInfo;
			SetFarmedSummonInfo( i, 0 );

			return true;
		}
	}

	return false;
}

bool StructPlayer::NurseSummon( AR_HANDLE hHandle )
{
	StructItem *pCard = StructItem::FindItem( hHandle );
	if( !pCard ) return false;

	// 맡겨지지 않은 카드
	if( !pCard->GetInstanceFlag().IsOn( ItemInstance::ITEM_FLAG_FARMED_SUMMON ) )
		return false;

	for( int i = 0; i < GameRule::FARM_MAX_COUNT; ++i )
	{
		FARMED_SUMMON_INFO *pInfo = m_vFarmedSummonInfo[i];
		if( !pInfo ) continue;

		if( pInfo->item->GetItemUID() == pCard->GetItemUID() )
		{
			time_t tCurrent = time( NULL );
			struct tm tmRefreshTime;
			localtime_s( &tmRefreshTime, &tCurrent );
			tmRefreshTime.tm_hour = 6;
			tmRefreshTime.tm_min = 0;
			tmRefreshTime.tm_sec = 0;
			tmRefreshTime.tm_isdst = -1;
			time_t tRefresh = mktime( &tmRefreshTime );

			// 오늘의 충전 시점을 지나지 않았다면 어제의 충전 시점과 비교
			if( tCurrent < tRefresh )
			{
				--tmRefreshTime.tm_mday;
				tRefresh = mktime( &tmRefreshTime );
			}

			if( pInfo->nursing_time >= tRefresh )
				return false;

			DBQuery( new DB_UpdateNursingTime( this, pInfo->farm_sid, tCurrent ) );
			pInfo->nursing_time = tCurrent;

			LOG::Log11N4S( LM_FARM_NURSE, GetAccountID(), GetSID(), pCard->GetItemCode(), pInfo->farm_sid, pCard->GetSummonSID(), pCard->GetSummonCode(), pCard->GetSummonStruct()->GetLevel(), pCard->GetSummonStruct()->GetEXP(), 0, 0, pCard->GetItemUID(), GetAccountName(), LOG::STR_NTS, GetName(), LOG::STR_NTS, pCard->GetSummonStruct()->GetName(), LOG::STR_NTS, "", 0 );
			return true;
		}
	}

	return false;
}

size_t StructPlayer::GetFarmedSummonCount()
{
	size_t count = 0;
	for( int i = 0; i < GameRule::FARM_MAX_COUNT; ++i )
	{
		if( m_vFarmedSummonInfo[i] ) 
			++count; 
	}

	return count;
}

size_t StructPlayer::GetNonCashFarmedSummonCount()
{
	size_t count = 0;
	for( int i = 0; i < GameRule::FARM_MAX_COUNT; ++i ) 
	{
		if( m_vFarmedSummonInfo[i] && !m_vFarmedSummonInfo[i]->is_cash )
			++count; 
	}

	return count;
}

int StructPlayer::GetFarmedSummonLevel( AR_HANDLE handle )
{
	int nLevel = 1;
	for( int i = 0; i < GameRule::FARM_MAX_COUNT; ++i )
	{
		FARMED_SUMMON_INFO *pInfo = m_vFarmedSummonInfo[i];
		if( !pInfo ) continue;

		if( pInfo->item->GetHandle() == handle )
		{
			StructSummon *pSummon = pInfo->item->GetSummonStruct();

			if( !pSummon )
			{
				assert( 0 );
				break;
			}

			time_t tHour = ( std::min( time( NULL ), pInfo->registration_time + pInfo->duration ) - pInfo->registration_time ) / 3600;
			double dRate = pInfo->is_using_cracker ? 1.5 : 1.0;
			__int64 nExp = 0;
			__int64 nExpLimit = 0;
			int nLevelLimit = std::min( pInfo->max_level, GameRule::FARM_MAX_LEVEL );

			switch( pSummon->GetTransformLevel() )
			{
			case SummonBase::EVOLVE_NORMAL:
				if( pInfo->is_cash )
					nExp = tHour * GameRule::nPremiumFarmNormalSummonEXP * dRate;
				else
					nExp = tHour * GameRule::nFarmNormalSummonEXP * dRate;
				nLevelLimit = std::min( nLevelLimit, GameRule::NORMAL_SUMMON_MAX_LEVEL );
				break;
			case SummonBase::EVOLVE_GROWTH:
				if( pInfo->is_cash )
					nExp = tHour * GameRule::nPremiumFarmGrowthSummonEXP * dRate;
				else
					nExp = tHour * GameRule::nFarmGrowthSummonEXP * dRate;
				nLevelLimit = std::min( nLevelLimit, GameRule::GROWTH_SUMMON_MAX_LEVEL );
				break;
			case SummonBase::EVOLVE_EVOLVE:
				if( pInfo->is_cash )
					nExp = tHour * GameRule::nPremiumFarmEvolveSummonEXP * dRate;
				else
					nExp = tHour * GameRule::nFarmEvolveSummonEXP * dRate;
				nLevelLimit = std::min( nLevelLimit, GameRule::EVOLVE_SUMMON_MAX_LEVEL );
				break;
			default:
				assert( 0 );
				return nLevel;
			}

			nExpLimit = GameContent::GetNeedSummonExp( nLevelLimit ) - 1;
			if( nExpLimit > pSummon->GetEXP() ) nExp = std::min( nExp, nExpLimit - pSummon->GetEXP() );
			else nExp = 0;

			nExp += pSummon->GetEXP();

			while( GameContent::GetNeedSummonExp( nLevel ) <= nExp )
			{
				if( nLevel >= GameRule::FARM_MAX_LEVEL ) break;
				++nLevel;
			}

			return nLevel;
		}
	}

	return nLevel;
}

void StructPlayer::MountSummon( AR_HANDLE handle )
{
	AR_TIME t = GetArTime();

	do
	{
		StructSummon * pRideSummon = NULL;

		if( m_nRidingStateCode )
			break;

		if( m_nRideIdx != MOUNT_NOTHING )
			break;

		if( !IsMountable( true ) )
			break;

		if( m_pMainSummon && m_pMainSummon->GetHandle() == handle && m_pMainSummon->IsRidable() )
		{
			pRideSummon = m_pMainSummon;
			m_nRideIdx = MOUNT_ON_MAIN;
		}
		else if( m_pSubSummon && m_pSubSummon->GetHandle() == handle && m_pSubSummon->IsRidable() )
		{
			pRideSummon = m_pSubSummon;
			m_nRideIdx = MOUNT_ON_SUB;
		}

		if( !pRideSummon )
			break;

		// pRideSummon->CalculateStat();

		// 크리처의 위치가 아닌 주인의 위치에서 라이딩이 되도록 변경 (2013-11-25)
		ARCADIA_LOCK( ArcadiaServer::Instance().LockObjects( this, pRideSummon ) );
		
		CalculateStat();
		
		TS_SC_MOUNT_SUMMON msg;

		msg.handle = GetHandle();
		msg.summon_handle = handle;
		msg.x = GetX();
		msg.y = GetY();
		msg.success = true;

		ArcadiaServer::Instance().Broadcast( GetRX(), GetRY(), GetRX(), GetRY(), GetLayer(), &msg );

		if( IsHiding() )
		{
			RemoveState( StructState::HIDE, GameRule::MAX_STATE_LEVEL );
			RemoveState( StructState::TRACE_OF_FUGITIVE, GameRule::MAX_STATE_LEVEL );
		}

		UpdateTitleConditionBySummonMount( pRideSummon->GetParentCard()->GetSummonCode(), true );

		return;
	} while( false );

	TS_SC_MOUNT_SUMMON msg;

	msg.handle = GetHandle();
	msg.summon_handle = handle;
	msg.x = 0;
	msg.y = 0;
	msg.success = false;

	PendMessage( this, &msg );
}

void StructPlayer::UnMount( const char flag, StructCreature * pCauser )
{
	if( m_nRidingStateCode )
	{
		RemoveState( m_nRidingStateCode );
	}
	else
	{
		if( !m_nRideIdx )
			return;

		StructSummon * pSummon = GetRideObject();

		// 더블 락 걸림. 락 위로 빼야 함.

		if( IsMoving() ) 
		{
			AR_TIME t = GetArTime();
			ArPosition pos = GetCurrentPosition( t );
			ArcadiaServer::Instance().SetMove( this, pos, pos, 0, true, t );
			ArcadiaServer::Instance().SetMove( GetRideObject(), pos, pos, 0, true, t, false );
		}

		TS_SC_UNMOUNT_SUMMON msg;

		msg.handle = GetHandle();
		msg.summon_handle = GetRideHandle();
		msg.flag = flag;

		ArcadiaServer::Instance().Broadcast( GetRX(), GetRY(), GetLayer(), &msg );

		m_nRideIdx = MOUNT_NOTHING;

		CalculateStat();

		if( pSummon )
		{
			// pSummon->CalculateStat();

			UpdateTitleConditionBySummonMount( pSummon->GetParentCard()->GetSummonCode(), false );
		}
	}

	if( flag == UNMOUNT_FALL && !IsDead() )
	{
		AR_TIME t = GetArTime();
		AddState( StructState::FALL_FROM_SUMMON, 0, 1, t, t + 300 );

		assert( pCauser );
		if( pCauser )
		{
			int prev_hp = GetHP();
			damage( pCauser, GetMaxHP() * GameRule::UNMOUNT_PENALTY );
			BroadcastHPMPMsg( this, GetHP() - prev_hp, 0 );
		}
	}
}

AR_HANDLE StructPlayer::GetRideHandle()
{
	switch( m_nRideIdx )
	{
	case MOUNT_ON_MAIN:
		return m_pMainSummon->GetHandle();
	case MOUNT_ON_SUB:
		return m_pSubSummon->GetHandle();
	default:
		return NULL;
	}
}

struct StructSummon * StructPlayer::GetRideObject() const
{
	switch( m_nRideIdx )
	{
	case MOUNT_ON_MAIN:
		return m_pMainSummon;
	case MOUNT_ON_SUB:
		return m_pSubSummon;
	default:
		return NULL;
	}
}

void StructPlayer::ChangeLocation( AR_UNIT x, AR_UNIT y, bool bByRequest, bool bBroadcast )
{
	ArcadiaAutoLock _lock;

	if( bByRequest )
	{
		_lock.set( ArcadiaServer::Instance().LockObjectWithVisibleRange( this ) );
	}

	ArPosition client_pos( x, y );
	ArPosition pos = GetCurrentPosition( GetArTime() );

	// 오차범위 안쪽이면 그냥 인정한다.
	if( client_pos.GetDistance( pos ) < GameRule::CHANGE_LOCATION_ERROR_RANGE ) 
	{
		pos = client_pos;
	}

	int location_id = GameContent::GetLocationId( pos.x, pos.y );

	TS_SC_CHANGE_LOCATION msg;

	msg.prev_location_id = GetLocationId();
	msg.cur_location_id = location_id;

	PendMessage( this, &msg );

	if( GetLocationId() != location_id )
	{
		if( GetLocationId() )
		{
			WorldLocationManager::Instance().RemoveFromLocation( this );
			m_pWorldLocation = NULL;
		}

		if( location_id )
		{
			m_pWorldLocation = WorldLocationManager::Instance().AddToLocation( location_id, this );

			if( ( IsPKOning() || ( IsPKOn() && !IsPKOffing() ) ) && !IsInPKField() )
			{
				bool bImmediateOffRequired = ( m_pWorldLocation->location_type == StructWorldLocation::LOCATION_DEATHMATCH
					|| m_pWorldLocation->location_type == StructWorldLocation::LOCATION_BATTLE_ARENA );

				TurnOffPkMode( bImmediateOffRequired );
			}

			if( bBroadcast && IsDungeonOriginalOwner() )
			{
				BroadcastStatusMessage( this ); // need to modify
			}

			if( GetCompeteID() && ( !IsInField() && !IsInBattleField() ) )
			{
				CompeteManager::Instance().RetireCompeteWithPlayer( this, COMPETE_END_BY_ENTERING_SAFETY_ZONE );
			}
		}

		setLocation( location_id );
	}
}

const bool StructPlayer::isInLocationType( unsigned char nLocationType ) const
{
	return ( m_pWorldLocation && m_pWorldLocation->location_type == nLocationType );
}

const bool StructPlayer::IsInPKField() const
{
	if( m_pWorldLocation )
	{
		if( m_pWorldLocation->location_type == StructWorldLocation::LOCATION_TOWN
			|| m_pWorldLocation->location_type == StructWorldLocation::LOCATION_NON_PK_FIELD
			|| m_pWorldLocation->location_type == StructWorldLocation::LOCATION_BATTLE_FIELD
			|| m_pWorldLocation->location_type == StructWorldLocation::LOCATION_EVENTMAP
			|| m_pWorldLocation->location_type == StructWorldLocation::LOCATION_HUNTAHOLIC_LOBBY
			|| m_pWorldLocation->location_type == StructWorldLocation::LOCATION_HUNTAHOLIC_DUNGEON
			|| m_pWorldLocation->location_type == StructWorldLocation::LOCATION_MARICAT_MARKET 
			|| m_pWorldLocation->location_type == StructWorldLocation::LOCATION_DEATHMATCH
			|| m_pWorldLocation->location_type == StructWorldLocation::LOCATION_RAMADAN_PRAYER_HALL
			|| m_pWorldLocation->location_type == StructWorldLocation::LOCATION_BATTLE_ARENA )
		{
			return false;
		}
	}

	// 무저갱 PK 불가
	if( GetLocationId() == StructWorldLocation::LOCATION_ID_ABADON )
	{
		return false;
	}

	return true;
}

const bool StructPlayer::IsInTown() const
{
	return isInLocationType( StructWorldLocation::LOCATION_TOWN ) || isInLocationType( StructWorldLocation::LOCATION_MARICAT_MARKET );
}

const bool StructPlayer::IsInField() const
{
	return isInLocationType( StructWorldLocation::LOCATION_FIELD );
}

const bool StructPlayer::IsInBattleField() const
{
	return isInLocationType( StructWorldLocation::LOCATION_BATTLE_FIELD );
}

const bool StructPlayer::IsInEventmap() const
{
	return isInLocationType( StructWorldLocation::LOCATION_EVENTMAP );
}

void StructPlayer::AddStamina( int nStamina )
{
	int nPrevStamina = GetStamina();

	// 스태미너를 늘리는 경우(nStamina > 0)에는 회복 후 수치가 최대치 이상이면
	// 회복량을 회복 가능한 최대 수치로 조정. 그러나 감소(nStamina < 0)는 정상 처리되어야 함
	if( nStamina > 0 && nPrevStamina + nStamina > m_nMaxStamina )
	{
		nStamina = ( m_nMaxStamina > nPrevStamina ) ? m_nMaxStamina - nPrevStamina : 0;
	}

	if( !nStamina )
	{
		return;
	}

	m_nStamina += nStamina;

	// Validation stamina
	//if( GetStamina() > m_nMaxStamina )
	//{
	//	m_nStamina = m_nMaxStamina;
	//}
	if( GetStamina() < 0 )
	{
		m_nStamina = 0;
	}

	if( GetStamina() != nPrevStamina )
	{
		SendPropertyMessage( this, GetHandle(), "stamina", GetStamina() );
	}
}

int StructPlayer::GetStaminaRegenRate()
{
	int stamina_regen = GameRule::STAMINA_REGEN_IN_FIELD;

	if( IsUsingTent() )
	{
		stamina_regen = GameRule::STAMINA_REGEN_IN_TENT;
	}
	else if( IsInTown() )
	{
		if( GetCondition() == StructPlayer::CONDITION_GOOD )
		{
			stamina_regen = GameRule::STAMINA_REGEN_ON_GOOD;
		}
		else
		{
			stamina_regen = GameRule::STAMINA_REGEN_ON_AVERAGE;
		}
	}

	if( IsGaiaMember() )
		stamina_regen *= GameRule::SECROUTE_STAMINA_REGEN_BONUS_RATE;

	if( m_nStaminaRegenBonus )
		stamina_regen += m_nStaminaRegenBonus;

	if( m_nStaminaRegenRate != stamina_regen )
	{
		m_nStaminaRegenRate = stamina_regen;
		SendPropertyMessage( this, GetHandle(), "stamina_regen", stamina_regen );
	}

	return stamina_regen;
}

StructPlayer::CONDITION_INFO StructPlayer::GetCondition()
{
	if( GetStamina() < GameRule::BAD_CONDITION_LIMIT )
		return CONDITION_BAD;

	else if ( GetStamina() < GameRule::AVERAGE_CONDITION_LIMIT )
		return CONDITION_AVERAGE;

	return CONDITION_GOOD;
}

void StructPlayer::onEnergyChange()
{
	TS_SC_ENERGY msg;
	msg.handle = GetHandle();
	msg.energy = GetEnergyCount();

	ArcadiaServer::Instance().Broadcast( GetRX(), GetRY(), GetLayer(), &msg );
}

void StructPlayer::EnumSummonPassiveSkill( SKILL_FUNCTOR & _fo ) const
{
	std::vector< StructSkill * >::const_iterator	it;
	for( it = m_vSummonPassiveSkillList.begin(); it != m_vSummonPassiveSkillList.end(); ++it )
	{
		_fo.onSkill( (*it) );
	}
}

void StructPlayer::EnumSummonAmplifyPassiveSkill( SKILL_FUNCTOR & _fo ) const
{
	std::vector< StructSkill * >::const_iterator	it;
	for( it = m_vAmplifySummonPassiveSkillList.begin(); it != m_vAmplifySummonPassiveSkillList.end(); ++it )
	{
		_fo.onSkill( (*it) );
	}
}

void StructPlayer::procMoveSpeedChangement()
{
	if( !IsMoving() )
		return;

	ArPosition pos = GetCurrentPosition( GetArTime() );

	// 이동 불가가 되었다면 현재 위치에 정지
	if( !IsMovable() )
	{
		ArcadiaServer::Instance().SetMove( this, pos, pos, 0, true, GetArTime() );
	}
	// 이동 중이었고 이동 가능 상태라면 현재 이동 속도와 스텟상의 이동 속도가 다를 경우 다시 이동 시킴
	else
	{
		unsigned char nSpeed = GetRealMoveSpeed();

		// 라이딩 소환수 탑승 상태라면 소환수의 라이딩 이속 적용
		if( GetRideObject() || HasRidingState() )
		{
			nSpeed = GetRealRidingSpeed();
		}

		if( GetSpeed() != nSpeed )
		{
			const std::vector< ArMoveVector::MOVE_INFO > & vMoveVector( GetTargetPosList() );
			std::vector< ArPosition > vMovePos;

			for( std::vector< ArMoveVector::MOVE_INFO >::const_iterator it = vMoveVector.begin() ; it != vMoveVector.end() ; ++it )
				vMovePos.push_back( (*it).end );

			ArcadiaServer::Instance().SetMultipleMove( this, pos, vMovePos, nSpeed, true, GetArTime() );
		}
	}
}

void StructPlayer::onAfterAddState( StructState & state )
{
	// 크리처가 효과받는 지속효과 부여시 크리처 재계산
	if( state.GetEffectType() == StructState::EF_CREATURE_PARAMETER_AMP )
	{
		setSummonUpdate();
	}

	StructCreature::onAfterAddState( state );
}

void StructPlayer::onAfterRemoveState( StructState & state, const bool bByDead )
{
	// 사망 시에는 부여된 지속효과 목록에서는 제거가 되지만 DB에는 존재하여야 부활에 의한 복구 기능이 동작할 수 있다.
	// 따라서 만료 처리를 하지 않고 removeStateByDead에서 사후 처리한다.
	if( !bByDead )
		state.Expire( this );

	// 크리처가 효과받는 지속효과 부여시 크리처 재계산
	if( state.GetEffectType() == StructState::EF_CREATURE_PARAMETER_AMP )
	{
		setSummonUpdate();
	}
	else if( state.GetEffectType() == StructState::EF_IMMORTALIZE )
	{
		int nHealingHP = state.GetValue( 2 ) > state.GetValue( 3 ) * m_nMaxHP ? state.GetValue( 2 ) : state.GetValue( 3 ) * m_nMaxHP;
		AddHP( nHealingHP );

		BroadcastHPMPMsg( this, nHealingHP, 0 );
	}
	else if( state.GetEffectType() == StructState::EF_INFINITE_SUMMON_TIME )
	{
		if( GetSubSummon() )
		{
			AR_TIME t = GetArTime();
			AR_TIME nNextUnSummonTime = state.GetValue( 0 ) * 100;

			TS_SC_UNSUMMON_NOTICE msg;

			msg.summon_handle = m_pSubSummon->GetHandle();
			msg.unsummon_duration = nNextUnSummonTime;

			PendMessage( this, &msg );

			SetNextUnSummonTime( t + nNextUnSummonTime );
		}
	}
	else if( state.GetEffectType() == StructState::EF_AUTO_RESURRECTION_AFTER_REMOVE_STATE )
	{
		int nCostMP = ( state.GetValue( 0 ) + state.GetValue( 1 ) * state.GetLevel() ) * GetMP();
		int nIncHP = (state.GetValue( 2 ) + state.GetValue( 3 ) * state.GetLevel() ) * nCostMP;
		Resurrect( CRT_STATE, nIncHP, -nCostMP, GetLastDecreasedEXP(), true );
	}

	StructCreature::onAfterRemoveState( state );
}

int StructPlayer::GetUnMountProbabilityOnDamage()
{
	int nProbability = 0;

	if( GetRideObject() )
	{
		nProbability = GetRideObject()->GetUnMountProbabilityOnDamage();
	}
	else if( m_nRidingStateCode )
	{
		StructState *pRideState = GetState( m_nRidingStateCode );
		if( pRideState )
		{
			nProbability = pRideState->GetValue( 6 );
		}
	}

	return nProbability;
}

int StructPlayer::GetUnMountProbabilityOnCriticalDamage() 
{
	int nProbability = 0;

	if( GetRideObject() )
	{
		nProbability = GetRideObject()->GetUnMountProbabilityOnCriticalDamage();
	}
	else if( m_nRidingStateCode )
	{
		StructState *pRideState = GetState( m_nRidingStateCode );
		if( pRideState )
		{
			nProbability = pRideState->GetValue( 7 );
		}
	}

	return nProbability;
}

bool StructPlayer::PutOnBelt( int pos, struct StructItem * pItem )
{
	if( pos >= m_nBeltSlotMax ) return false;

	if( m_aBeltSlotCard[pos] ) return false;

	if( pItem->IsSummonCard() )
	{
		// 빈카이거나 내구도가 다 한 소환수 카드
		if( !pItem->GetInstanceFlag().IsOn( ItemInstance::ITEM_FLAG_SUMMON ) || ( pItem->GetMaxEtherealDurability() && !pItem->GetCurrentEtherealDurability() ) )
			return false;
	}
	else if( !pItem->IsEquipmentOnBelt() )
	{
		return false;
	}

	int i;

	// 소환수 카드는 장착 시에 강화 제한이 있다.
	if( pItem->IsSummonCard() )
	{
		if( pos == 2 )
		{
			if( pItem->GetItemEnhance() < 1 ) return false;
		}
		else if( pos == 3 )
		{
			if( pItem->GetItemEnhance() < 2 ) return false;
		}
		else if (pos > 5)
		{
			return false;
		}
	}
	else if( pItem->IsEquipmentOnBelt() )
	{
		// 내구도 체크
		if( !pItem->IsWearable() ) return false;

		// 레벨 체크
		if( pItem->GetLevelLimit() > GetLevel() ) return false;
		if( pItem->GetItemBase().nMinLevel && GetLevel() < pItem->GetItemBase().nMinLevel ) return false;
		if( pItem->GetItemBase().nMaxLevel && GetLevel() > pItem->GetItemBase().nMaxLevel ) return false;
	}

	// 바인딩 된 카드는 안돼!
	for (i = 0; i < 6; ++i)
	{
		if (m_aBindSummonCard[i] == pItem) return false;
	}

	// 이미 장착된 카드
	if( pItem->IsSummonCard() )
	{
		for (i = 0; i < m_nBeltSlotMax; ++i)
		{
			if (m_aBeltSlotCard[i] == pItem) return false;
		}
	}
	// 보스 카드는 한 장만 착용 가능
	else if( pItem->IsEquipmentOnBelt() )
	{
		for (i = 0; i < m_nBeltSlotMax; ++i)
		{
			if (m_aBeltSlotCard[i] && (m_aBeltSlotCard[i]->GetItemCode() == pItem->GetItemCode())) return false;
		}
		if (pos < 6)
		{
			for (i = 0; i < 8; ++i)
			{
				if (m_aBeltSlotCard[i] && m_aBeltSlotCard[i]->IsEquipmentOnBelt()) return false;
			}
		}

		int curCount = pItem->GetCount();
		if (curCount == 1)
		{
			pItem->SetBindTarget(this, false);
		}
		else
		{
			pItem->SetCount(1);
			SendItemMessage(this, pItem);

			StructItem* pNewItem = StructItem::AllocItem(0, pItem->GetItemCode(), 1, ItemInstance::BY_DIVIDE, -1, -1, 0, 0, 0, 0, -1, 0, 0, 0, 0);
			pNewItem->CopyFrom(pItem);
			pNewItem->SetIdx(0);
			pNewItem->SetCount(1);
			pNewItem->SetBindTarget(this);
			PushItem(pNewItem, curCount - 1);
		}
	}

	m_aBeltSlotCard[pos] = pItem;

	// 호칭을 위해 동일한 코드의 소환수 카드를 몇 개나 장착했는지 검사
	std::vector< int > vBeltCode;
	for( int i = 0; i < m_nBeltSlotMax; ++i )
	{
		if( m_aBeltSlotCard[i] )
			vBeltCode.push_back( m_aBeltSlotCard[i]->GetSummonCode() );
	}

	UpdateTitleConditionBySummonPutOnBelt( vBeltCode );

	return true;
}

void StructPlayer::PutOffBelt( int pos )
{
	if( pos >= 8 )
		return;

	m_aBeltSlotCard[pos] = 0;

	// 호칭을 위해 동일한 코드의 소환수 카드를 몇 개나 장착했는지 검사
	std::vector< int > vBeltCode;
	for( int i = 0; i < m_nBeltSlotMax; ++i )
	{
		if( m_aBeltSlotCard[i] )
			vBeltCode.push_back( m_aBeltSlotCard[i]->GetSummonCode() );
	}

	UpdateTitleConditionBySummonPutOnBelt( vBeltCode );
}

const bool StructPlayer::ClearExpiredItem()
{
	// 사본을 받아서 쓰지 않으면 clearExpiredItem에서 멤버가 삭제되서 iterator가 무효화 됨
	std::vector< StructItem * > vExpireItemList = m_Inventory.GetExpireItemList();
	bool bExistClearedItem = clearExpiredItem( vExpireItemList, false );

	// 창고 아이템도 만료되면 삭제 처리되게 변경 (2014-02-28)
	if( IsUsingStorage() )
	{
		vExpireItemList = m_Storage.GetExpireItemList();
		bExistClearedItem = clearExpiredItem( vExpireItemList, true ) || bExistClearedItem;
	}

	return bExistClearedItem;
}

const bool StructPlayer::clearExpiredItem( const std::vector< struct StructItem * > & vExpireItemList, bool bStorage )
{
	// 현재 구현상으로는 아이템 착용자가 소환수였을 경우 지역락 문제 있으므로 수정 필요(장비 소멸 시, 소환되어 있던 소환수의 카드 소멸 시 둘 다 착용 해제 발생)
	// * 만료된 아이템에 의해 영향을 받은 소환수들에게 SetNeedCalculateStat을 호출하게 수정해뒀지만, 차후에 지역락 문제를 수정하게 되면 아이템 착용 해제와 함께
	//   해당 지역락이 걸린 상태에서 바로 CalculateStat을 호출하게 변경해야 함

	time_t cur_t = time( NULL );
	bool ret = false;
	bool bNeedToBroadcastEquipMessage = false;

	for( std::vector< StructItem * >::const_iterator it = vExpireItemList.begin(); it != vExpireItemList.end(); ++it )
	{
		StructItem * pItem = (*it);

		if( pItem->GetExpireTime() < cur_t )
		{
			ret = true;

			if( IsBoothOpen() )
			{
				THREAD_SYNCRONIZE( StructPlayer::GetBoothLock() );

				for( std::vector< StructPlayer::BOOTH_ITEM_INFO >::iterator itBoothItem = m_vBoothItem.begin(); itBoothItem != m_vBoothItem.end(); ++itBoothItem )
				{
					if( (*itBoothItem).pItem == pItem )
					{
						vector_fast_erase( &m_vBoothItem, itBoothItem );

						broadcastBoothInfo();

						break;
					}
				}
			}

			if( pItem->GetWearInfo() != -1 )
			{
				StructCreature::iterator itCreature = StructCreature::get( pItem->GetBindedCreatureHandle() );
				if( (*itCreature) ) StructCreature::putoffItem( (*itCreature), pItem->GetWearInfo() );

				if( (*itCreature) == this && pItem->GetWearInfo() < ItemBase::MAX_ITEM_WEAR )
				{
					bNeedToBroadcastEquipMessage = true;
				}
				else if( (*itCreature)->IsSummon() )
				{
					(*itCreature)->SetNeedCalculateStat();
				}
			}
			else if( pItem->IsSummonCard() )
			{
				for( int i = 0; i < 8; ++i )
				{
					if( GetSummonCardAt( i ) == pItem )
					{
						StructSummon * pSummon = pItem->GetSummonStruct();

						if( pSummon )
						{
							if( pSummon->IsInWorld() )
							{
								PendUnSummon( pSummon );
								ArcadiaServer::Instance().SetObjectPriority( this, ArSchedulerObject::UPDATE_PRIORITY_HIGHEST );
							}

							// 소환수가 입고 있던 장비 해제
							for( int x= 0; x < ItemBase::MAX_ITEM_WEAR; ++x )
							{
								StructItem *pItem = pSummon->GetWearedItem( (ItemBase::ItemWearType)x );
								if( pItem ) 
								{
									pSummon->Putoff( (ItemBase::ItemWearType)x );
								}
							}

							SetSummonCardAt( i, NULL );

							pSummon->SetNeedCalculateStat();
						}

						SendCreatureEquipMessage( this, false );

						UpdateTitleConditionBySummonEquip( pItem->GetSummonCode(), GameContent::GetSummonInfo( pItem->GetSummonCode() )->rate, pItem->GetItemEnhance(), false );

						break;
					}
					else if( GetBeltSlotCardAt( i ) == pItem )
					{
						PutOffBelt( i );

						SendBeltSlotInfo( this );

						break;
					}
				}
			}
			else if( pItem->IsPetCage() )
			{
				StructPet *pPet = pItem->GetPetStruct();
				if( pPet && pPet->IsInWorld() && GetSummonedPet() == pPet )
				{
					UnSummonPet();
				}
			}

			AR_HANDLE hItem = pItem->GetHandle();
			int nItemEnhanceAndLevel = pItem->GetItemEnhance() * 100 + pItem->GetItemLevel();
			ItemBase::ItemCode eItemCode = pItem->GetItemCode();
			__int64 nItemCount = pItem->GetCount();
			ItemUID nItemUID = pItem->GetItemUID();
			bool bLogRequiredOnExpiration = pItem->GetItemBase().bLogRequiredOnExpiration;

			bool bSuccess = false;
			char *szType = "INVENTORY_EXPIRE";
			char *szMsg = "ITEM_EXPIRE|%d";
			if( bStorage )
			{
				szType = "STORAGE_EXPIRE";
				szMsg = "ITEM_EXPIRE_STORAGE|%d";
				bSuccess = EraseItemFromStorage( pItem, nItemCount );
			}
			else
			{
				bSuccess = EraseItem( pItem, nItemCount );
			}

			if( bSuccess )
			{
				PrintfChatMessage( false, CHAT_NOTICE, "@SYSTEM", this, szMsg, pItem->GetItemCode() );
				if( bLogRequiredOnExpiration )
				{
					LOG::Log11N4S( LM_ITEM_EXPIRATION, GetAccountID(), GetSID(), nItemEnhanceAndLevel, eItemCode, nItemCount, 0, 0, 0, GetX(), GetY(), nItemUID, GetAccountName(), LOG::STR_NTS, GetName(), LOG::STR_NTS, "", 0, szType, LOG::STR_NTS );
				}
			}
		}
	}

	if( bNeedToBroadcastEquipMessage )
	{
		TS_WEAR_INFO msg;
		GetWearMsg( this, msg );
		ArcadiaServer::Instance().Broadcast( GetRX(), GetRY(), GetLayer(), &msg );
	}

	return ret;
}

const bool StructPlayer::ClearExpiredElementalEffect()
{
	time_t cur_t = time( NULL );

	bool bExistClearedItem = false;

	bool bNeedToBroadcastEquipMessage = false;

	for( std::vector< StructItem * >::iterator it = m_vElementalEffectedItem.begin(); it != m_vElementalEffectedItem.end(); /* 루프에서 ++it 처리 */ )
	{
		assert( (*it)->GetElementalEffectExpireTime() < cur_t || (*it)->GetElementalEffectType() );

		if( (*it)->GetElementalEffectExpireTime() < cur_t )
		{
			(*it)->ClearElementalEffect();
			(*it)->TurnOnUpdateFlag();

			LOG::Log11N4S( LM_ITEM_ELEMENT_EXPIRE,
				GetAccountID(), GetSID(), // N1, N2
				(*it)->GetItemEnhance() * 100 + (*it)->GetItemLevel(), (*it)->GetItemCode(), // N3, N4
				(*it)->GetElementalEffectType(), (*it)->GetElementalEffectAttackPoint(), // N5, N6
				(*it)->GetElementalEffectMagicPoint(), 0, // N7, N8
				GetX(), GetY(), (*it)->GetItemUID(), // N9, N10, N11
				GetAccountName(), LOG::STR_NTS,
				GetName(), LOG::STR_NTS,
				"", 0,
				"", 0 );

			bExistClearedItem = true;

			SendItemMessage( this, (*it) );

			if( IsBoothOpen() )
			{
				THREAD_SYNCRONIZE( StructPlayer::GetBoothLock() );

				for( std::vector< StructPlayer::BOOTH_ITEM_INFO >::iterator itBoothItem = m_vBoothItem.begin(); itBoothItem != m_vBoothItem.end(); ++itBoothItem )
				{
					if( (*itBoothItem).pItem == (*it) )
					{
						broadcastBoothInfo();

						break;
					}
				}
			}

			if( (*it)->GetWearInfo() != -1 && (*it)->GetWearInfo() < ItemBase::MAX_ITEM_WEAR )
			{
				bNeedToBroadcastEquipMessage = true;
			}

			PrintfChatMessage( false, CHAT_NOTICE, "@SYSTEM", this, "@???\v#@item_name@#\v@%d", (*it)->GetItemBase().nNameId );

			it = m_vElementalEffectedItem.erase( it );
		}
		else
		{
			++it;
		}
	}

	if( bNeedToBroadcastEquipMessage )
	{
		TS_WEAR_INFO msg;
		GetWearMsg( this, msg );
		ArcadiaServer::Instance().Broadcast( GetRX(), GetRY(), GetLayer(), &msg );
	}

	return bExistClearedItem;
}

void StructPlayer::AddToElementalEffectedItemList( StructItem * pItem )
{
	// 이미 속성 이펙트 부여된 걸로 처리되고 있던 아이템이면 패스
	if( std::find( m_vElementalEffectedItem.begin(), m_vElementalEffectedItem.end(), pItem ) != m_vElementalEffectedItem.end() )
		return;

	m_vElementalEffectedItem.push_back( pItem );
}

void StructPlayer::RemoveFromElementalEffectedItemList( StructItem * pItem )
{
	std::vector< StructItem * >::iterator itErase = std::find( m_vElementalEffectedItem.begin(), m_vElementalEffectedItem.end(), pItem );
	if( itErase != m_vElementalEffectedItem.end() )
	{
		m_vElementalEffectedItem.erase( itErase );
	}
}

AR_TIME StructPlayer::GetItemCoolTime( int group_id )
{
	if( group_id < 1 || group_id > ItemBase::MAX_COOLTIME_GROUP ) return 0;

	return m_nItemCoolTime[ group_id - 1 ];
}

int StructPlayer::IsUseableItem( struct StructItem *pItem, struct StructCreature * pTarget )
{
	if( pItem->GetCoolTimeGroup() < 0 || pItem->GetCoolTimeGroup() > ItemBase::MAX_COOLTIME_GROUP ) return RESULT_COOL_TIME;

	if( pItem->GetCoolTimeGroup() && m_nItemCoolTime[ pItem->GetCoolTimeGroup() - 1 ] > GetArTime() ) return RESULT_COOL_TIME;

	if( pItem->GetBaseFlag().IsOn( ItemBase::FLAG_CANT_USE_OVERWEIGHT ) && (float) GetWeight() / GetMaxWeight() > 0.5f )
	{
		return RESULT_TOO_HEAVY;
	}

	if( ( IsRiding() || HasRidingState() ) && pItem->GetBaseFlag().IsOn( ItemBase::FLAG_CANT_USE_RIDING ) )
	{
		return RESULT_LIMIT_RIDING;
	}

	if( pItem->GetItemBase().nMaxLevel && pItem->GetItemBase().nMaxLevel < GetLevel() ) return RESULT_LIMIT_MAX;
	if( pItem->GetItemBase().nMinLevel > GetLevel() ) return RESULT_LIMIT_MIN;

	if( pTarget )
	{
		if( pItem->GetItemBase().nTargetMaxLevel && pItem->GetItemBase().nTargetMaxLevel < pTarget->GetLevel() ) return RESULT_LIMIT_MAX;
		if( pItem->GetItemBase().nTargetMinLevel && pItem->GetItemBase().nTargetMinLevel > pTarget->GetLevel() ) return RESULT_LIMIT_MIN;
	}

	// 직업 차수 체크
	if( ( 1 << GetJobDepth() & pItem->GetItemBase().nJobDepth ) == 0 ) return RESULT_LIMIT_JOB;

	return RESULT_SUCCESS;
}

void StructPlayer::SetHuntaholicEnterableCount( const int nEnterableCount )
{
	m_nHuntaholicEnterableCount = std::max( nEnterableCount, 0 );
	SendPropertyMessage( this, GetHandle(), "huntaholic_ent", m_nHuntaholicEnterableCount );
}

void StructPlayer::AddHuntaholicEnterableCount( const int nEnterableCountInc )
{
	m_nHuntaholicEnterableCount = std::max( m_nHuntaholicEnterableCount + nEnterableCountInc, 0 );
	SendPropertyMessage( this, GetHandle(), "huntaholic_ent", m_nHuntaholicEnterableCount );
}

bool StructPlayer::IsMixable( StructItem * pItem )
{
	return IsErasable( pItem );

	/*
	if( pItem->GetOwnerHandle() != GetHandle() )
	return false;

	if( pItem->GetWearInfo() != -1 )
	return false;

	if( pItem->IsSkillCard() && pItem->GetBindedCreatureHandle() )
	return false;

	for( int i = 0; i < 6; ++i )
	{
	if( pItem->IsSummonCard() )
	{
	if( m_aBeltSlotCard[i] && pItem == m_aBeltSlotCard[i] )
	return false;

	if( m_aBindSummonCard[i] && pItem == m_aBindSummonCard[i] )
	return false;
	}
	}

	if( pItem->IsInStorage() )
	return false;

	return true;
	*/
}

bool StructPlayer::IsErasable( StructItem * pItem )
{
	if( !pItem->IsInInventory() )
		return false;

	if( pItem->GetOwnerHandle() != GetHandle() )
		return false;

	if( pItem->GetWearInfo() != -1 )
		return false;

	if( pItem->IsSkillCard() && pItem->GetBindedCreatureHandle() )
		return false;

	if( pItem->IsSummonCard() )
	{
		// 월드에 소환수가 소환되어 있는 상태의 카드는 삭제 못 함
		if( pItem->GetSummonStruct() && pItem->GetSummonStruct()->IsInWorld() )
		{
			// 역소환 처리 대기 중인 소환수의 카드가 아니라면 문제가 있는 것일 수도 있지만
			//assert( ( m_pMainSummon == pItem->GetSummonStruct() && ( m_nPendingUnSummon & UNSUMMON_MAIN ) ) ||
			//	( m_pSubSummon == pItem->GetSummonStruct() && ( m_nPendingUnSummon & UNSUMMON_SUB ) ) );
			// 소환해 놓은 소환수의 카드를 조합하려고 하거나 상점에 판매하는 등 아이템 삭제에 대한 시도가 있을 수 있음
			// 이 경우에는 false를 리턴하는 것이 정상적인 경우이므로 assert를 걸 필요가 없음

			// 역소환 대기 중인 소환수의 카드는 역소환 처리 후에 다시 삭제 시도가 들어와야 함(대개 clearExpiredItem)
			return false;
		}

		for( int i = 0 ; i < 6 ; ++i )
		{
			if( m_aBeltSlotCard[i] && pItem == m_aBeltSlotCard[i] )
				return false;

			if( m_aBindSummonCard[i] && pItem == m_aBindSummonCard[i] )
				return false;
		}

		// 크리쳐 농장에 맡겼으면 삭제 못함
		if( pItem->GetInstanceFlag().IsOn( ItemInstance::ITEM_FLAG_FARMED_SUMMON ) )
			return false;
	}

	if( pItem->IsEquipmentOnBelt() )
	{
		for( int i = 0 ; i < 6 ; ++i )
		{
			if( m_aBeltSlotCard[i] && pItem == m_aBeltSlotCard[i] )
				return false;
		}
	}

	if( pItem->IsPetCage() && pItem->GetPetStruct() && pItem->GetPetStruct()->IsInWorld() )
		return false;

	if( pItem->IsInStorage() )
		return false;

	return true;
}

bool StructPlayer::IsDecomposable( StructItem * pItem )
{
	if( pItem->GetBaseFlag().IsOn( ItemBase::FLAG_CANT_DECOMPOSE ) )
	{
		return false;
	}

	// 여기서 플레이어와 아이템과의 관계 체크 (아이템이 착용중인지, 벨트에 장착중인지, 인벤에 있는지 등등 - )
	if( !IsErasable( pItem ) ) 
	{
		return false;
	}

	// 강화 실패작 체크
	if( pItem->GetInstanceFlag().IsOn( ItemInstance::ITEM_FLAG_FAILED ) ) 
	{
		return false;
	}

	// 에테리얼 내구도가 있을 경우 0이면 안됨. (크리쳐 카드 등 포함)
	if( pItem->GetMaxEtherealDurability() && !pItem->GetCurrentEtherealDurability() )	
	{
		return false;
	}

	// 이 크리쳐 카드를 이용하여 테이밍 시도 중이면 안됨.
	if( pItem->IsSummonCard() && pItem->GetInstanceFlag().IsOn( ItemInstance::ITEM_FLAG_TAMING ) ) 
	{
		return false;
	}

	return true;
}

bool StructPlayer::IsRandomizable( StructItem * pItem )
{
	// 애초에 랜덤화 불가 아이템이거나 이미 랜덤화 되있으면 안됨.
	if( !pItem->IsRandomizable() || pItem->IsIdentified() )
	{
		return false;
	}

	// 여기서 플레이어와 아이템과의 관계 체크 (아이템이 착용중인지, 벨트에 장착중인지, 인벤에 있는지 등등 - )
	if( !IsErasable( pItem ) ) 
	{
		return false;
	}

	// 강화 실패작 체크
	if( pItem->GetInstanceFlag().IsOn( ItemInstance::ITEM_FLAG_FAILED ) ) 
	{
		return false;
	}

	// 에테리얼 내구도가 있을 경우 0이면 안됨. 
	if( pItem->GetMaxEtherealDurability() && !pItem->GetCurrentEtherealDurability() )	
	{
		return false;
	}

	return true;
}

bool StructPlayer::IsSellable( StructItem * pItem )
{
	if( !IsErasable( pItem ) )
		return false;

	// 테이밍 시도 중인 소환수 카드는 판매 불가
	if( pItem->GetInstanceFlag().IsOn( ItemInstance::ITEM_FLAG_TAMING ) ) return false;

	if( pItem->GetBaseFlag().IsOn( ItemBase::FLAG_CANT_SELL ) ) return false;

	return true;
}

bool StructPlayer::IsTradable( StructItem * pItem )
{
	if( !IsErasable( pItem ) )
		return false;

	if( !pItem->IsTradable() ) return false;

	// 테이밍 시도 중인 소환수 카드는 거래 불가
	if( pItem->GetInstanceFlag().IsOn( ItemInstance::ITEM_FLAG_TAMING ) ) return false;

	if( pItem->GetInstanceFlag().IsOn( ItemInstance::ITEM_FLAG_FAILED ) ) return false;

	// 랜덤화되면 거래 불가 : 2014-01-13 랜덤 아이템 리뉴얼로 삭제
	// if( pItem->IsIdentified() ) return false;

	// 에테리얼 내구도에 의해 파괴된 아이템은 거래 불가
	if( pItem->GetMaxEtherealDurability() && !pItem->GetCurrentEtherealDurability() ) return false;

	return true;
}

bool StructPlayer::IsDropable( StructItem * pItem )
{
	if( !IsErasable( pItem ) )
		return false;

	if( !pItem->IsDropable() ) return false;

	if( pItem->GetInstanceFlag().IsOn( ItemInstance::ITEM_FLAG_FAILED ) ) return false;

	// 테이밍 시도 중인 소환수 카드는 못 버림
	if( pItem->GetInstanceFlag().IsOn( ItemInstance::ITEM_FLAG_TAMING ) ) return false;

	// 랜덤화 된 아이템은못 버림
	if( pItem->IsIdentified() ) return false;

	// 창고 아이템은 못버림
	if( pItem->IsInStorage() ) return false;

	// 캐쉬템 드랍 가능 플래그가 세팅되지 않으면 캐쉬 아이템은 못버림
	if( !GameRule::bCashItemDropable && pItem->IsCashItem() ) return false;

	return true;
}

bool StructPlayer::ProcGlobalChatProcess( const char * pChat, int nChatType )
{
	if( GetPermission() > 0 ) //GM 권한은 패널티 처리 안함.
		return true;

	TS_SC_CHAT_RESULT msg;
	msg.type = nChatType;

	if( m_nLastGlobalChatTime + 300 > GetArTime() )
	{
		msg.result = TS_SC_CHAT_RESULT::RESULT_COOLTIME;
		PendMessage( this, &msg );

		return false;
	}

	if ( GetLevel() < GameRule::nMinGlobalChatUsableLevel )
	{
		msg.result = TS_SC_CHAT_RESULT::RESULT_LEVEL_RESTRICT;
		msg.percentage = GameRule::nMinGlobalChatUsableLevel;
		PendMessage( this, &msg );

		return false;
	}

	if( nChatType == CHAT_ADV )
	{
		// 이녀석은 따로 돌아간다.-_-

		time_t t = time( NULL );

		if( m_nAdvChatCount )
		{
			struct tm local_tm;
			localtime_s( &local_tm, &m_nLastAdvChatTime );

			local_tm.tm_hour = 6;
			local_tm.tm_min = 0;
			local_tm.tm_sec = 0;
			local_tm.tm_isdst = -1;

			time_t renew_t = mktime( &local_tm );

			if( m_nLastAdvChatTime > renew_t )
				renew_t += 3600 * 24;

			if( t > renew_t )
				m_nAdvChatCount = 0;
		}

		if( GameRule::bLimitAdvChatCount && m_nAdvChatCount > GetLevel() / 10 )
		{
			msg.result = TS_SC_CHAT_RESULT::RESULT_COOLTIME;
			PendMessage( this, &msg );

			return false;
		}

		++m_nAdvChatCount;

		m_nLastAdvChatTime = t;
		m_nLastGlobalChatTime = GetArTime();

		return true;
	}

	int m = 1;

	if( nChatType == CHAT_GLOBAL )
		m = 2;

	if( m_strLastGlobalChat == pChat )
		m *= 2;

	if( m_nChatPenalty >= 5 )
	{
		float fRatio = ( pow( float( m_nChatPenalty - 4 ), 2 ) / 3 ) * 0.01f;
		int nHPDec = fRatio * GetMaxHP();
		int nMPDec = fRatio * GetMaxMP();
		msg.percentage = fRatio * 100;

		if( GetHP() <= nHPDec || GetMP() <= nMPDec )
		{
			msg.result = TS_SC_CHAT_RESULT::RESULT_NEED_MORE_HPMP;
			PendMessage( this, &msg );

			return false;
		}

		AddHP( 0 - nHPDec );
		AddMP( 0 - nMPDec );

		msg.result = TS_SC_CHAT_RESULT::RESULT_DECREASE_HPMP;
		PendMessage( this, &msg );

		ARCADIA_LOCK( ArcadiaServer::Instance().LockObjectWithVisibleRange( this ) );
		BroadcastHPMPMsg( this, 0 - nHPDec, 0 - nMPDec );

	}

	m_strLastGlobalChat = pChat;
	m_nChatPenalty += m;
	m_nNextChatPenaltyDecreaseTime = GetArTime() + 3000;
	m_nLastGlobalChatTime = GetArTime();

	return true;
}

void StructPlayer::SetImmoralPoint( const c_fixed10 & fIP )
{
	if( IsInSiegeOrRaidDungeon() )
		return;

	m_fImmoralPoint = fIP;
	SendPropertyMessage( this, GetHandle(), "immoral", GetImmoralPoint().get() );

	m_TitleManager.UpdateTitleConditionByImmoralPoint( fIP );
}

void StructPlayer::IncPKC()
{
	++m_nPKC;
	SendPropertyMessage( this, GetHandle(), "pk_count", m_nPKC );
}

void StructPlayer::IncDKC()
{
	++m_nDKC;
	SendPropertyMessage( this, GetHandle(), "dk_count", m_nDKC );
}

int StructPlayer::GetFavor( int favor_id ) const
{
	std::vector< FAVOR_INFO >::const_iterator it;

	for( it = m_vFavorInfo.begin(); it != m_vFavorInfo.end(); ++it )
	{
		if( favor_id == (*it).favor_id )
			return (*it).favor;
	}

	return 0;
}

void StructPlayer::AddFavor( int favor_id, int favor )
{
	std::vector< FAVOR_INFO >::iterator it;

	for( it = m_vFavorInfo.begin(); it != m_vFavorInfo.end(); ++it )
	{
		if( favor_id == (*it).favor_id )
		{
			(*it).favor += favor;
			(*it).bNeedToUpdate = true;
			return;
		}
	}

	m_vFavorInfo.push_back( FAVOR_INFO( favor_id, favor ) );
	DBQuery( new DB_InsertFavor( this, favor_id, favor ) );
}

void StructPlayer::SetFavor( int favor_id, int favor, bool bDBJob )
{
	std::vector< FAVOR_INFO >::iterator it;

	for( it = m_vFavorInfo.begin(); it != m_vFavorInfo.end(); ++it )
	{
		if( favor_id == (*it).favor_id )
		{
			(*it).favor = favor;
			(*it).bNeedToUpdate = true;

			return;
		}
	}

	m_vFavorInfo.push_back( FAVOR_INFO( favor_id, favor ) );

	if( bDBJob )
		DBQuery( new DB_InsertFavor( this, favor_id, favor ) );
}

const bool StructPlayer::IsInSiegeOrRaidDungeon() const
{
	int dungeon_id = DungeonManager::Instance().GetDungeonID( GetX(), GetY() );

	return ( dungeon_id && GetLayer() >= DungeonManager::DUNGEON_SIEGE_LAYER && GetLayer() < DungeonManager::CUSTOMIZE_DUNGEON_LAYER );
}

const bool StructPlayer::IsInSiegeDungeon() const
{
	return ( m_pWorldLocation && m_pWorldLocation->location_type == StructWorldLocation::LOCATION_DUNGEON && GetLayer() == DungeonManager::DUNGEON_SIEGE_LAYER );
}

const bool StructPlayer::IsInDungeon() const
{
	return ( DungeonManager::Instance().GetDungeonID( GetX(), GetY() ) != 0 );
}

const bool StructPlayer::IsInSecretDungeon() const
{
	return isInLocationType( StructWorldLocation::LOCATION_SECRET_DUNGEON );
}

const bool StructPlayer::IsInHuntaholic() const
{
	return ( m_pWorldLocation &&
		( m_pWorldLocation->location_type == StructWorldLocation::LOCATION_HUNTAHOLIC_LOBBY ||
		m_pWorldLocation->location_type == StructWorldLocation::LOCATION_HUNTAHOLIC_DUNGEON ) );
}

const bool StructPlayer::IsInInstanceDungeon() const
{
	return isInLocationType( StructWorldLocation::LOCATION_INSTANCE_DUNGEON );
}

const bool StructPlayer::IsInDeathmatch() const
{
	return isInLocationType( StructWorldLocation::LOCATION_DEATHMATCH );
}

const bool StructPlayer::IsInBattleArena() const
{
	return isInLocationType( StructWorldLocation::LOCATION_BATTLE_ARENA );
}

const bool StructPlayer::IsInRamadanPrayerHall() const
{
	return isInLocationType( StructWorldLocation::LOCATION_RAMADAN_PRAYER_HALL );
}

const bool StructPlayer::IsDungeonOriginalOwner() const
{
	int guild_id = PartyManager::GetInstance().GetAttackTeamGuildID( GetPartyID() );
	return ( guild_id && IsInSiegeDungeon() && guild_id == DungeonManager::Instance().GetOriginalOwnGuildID( DungeonManager::Instance().GetDungeonID( GetX(), GetY() ) ) );
}

const bool StructPlayer::IsDungeonOriginalSieger() const
{
	int guild_id = PartyManager::GetInstance().GetAttackTeamGuildID( GetPartyID() );
	return ( guild_id && IsInSiegeDungeon() && guild_id != DungeonManager::Instance().GetOriginalOwnGuildID( DungeonManager::Instance().GetDungeonID( GetX(), GetY() ) ) );
}

bool StructPlayer::IsInSecroute() const
{
	return ( GetLocationId() == StructWorldLocation::LOCATION_ID_SECROUTE_1 ||
		GetLocationId() == StructWorldLocation::LOCATION_ID_SECROUTE_2 ||
		GetLocationId() == StructWorldLocation::LOCATION_ID_SECROUTE_AUCTION );
}

bool StructPlayer::ResurrectByPotion()
{
	StructItem * pPotion = FindItem( ItemBase::ItemCode( ItemBase::ITEM_CODE_GRAND_FAIRY_POTION_NOT_FOR_SALE ) );

	if( !pPotion )
	{
		pPotion = FindItem( ItemBase::ItemCode( ItemBase::ITEM_CODE_GRAND_FAIRY_POTION_BY_PLAYPOINT ) );
	}
	if( !pPotion )
	{
		pPotion = FindItem( ItemBase::ItemCode( ItemBase::ITEM_CODE_GRAND_FAIRY_POTION ) );
	}
	if( !pPotion )
	{
		pPotion = FindItem( ItemBase::ItemCode( ItemBase::ITEM_CODE_FAIRY_POTION ) );
	}

	if( !pPotion )
		return false;

	const ItemBase & base = pPotion->GetItemBase();

	for( int x = 0; x < ItemBase::MAX_OPTION_NUMBER; ++x )
	{
		if( base.nOptType[x] && base.nOptType[x] == ITEM_EFFECT_CHARM::FAIRY_POTION ) 
		{
			int prev_hp = GetHP();

			// 사망시 없어진 지속효과를 복원하기 위해 우선 살려놓고
			AddHP( 1 );

			RestoreRemovedStateByDead();

			float var1 = base.fOptVar1[x];
			float var2 = base.fOptVar2[x];

			AddHP( GetMaxHP() * var1 - 1 );

			__int64 nRecoveryEXP = GetLastDecreasedEXP() * var2;

			SetEXP( GetEXP() + nRecoveryEXP );

			LOG::Log11N4S( LM_ITEM_USE, GetAccountID(), GetSID(), pPotion->GetItemEnhance() * 100 + pPotion->GetItemLevel(), pPotion->GetItemCode(), 1, pPotion->GetCount() - 1, 0, 0, 0, 0, pPotion->GetItemUID(), GetAccountName(), LOG::STR_NTS, GetName(), LOG::STR_NTS, "", 0, "", 0 );

			EraseItem( pPotion, 1 );

			Save();

			BroadcastHPMPMsg( this, GetHP() - prev_hp, 0 );

			if( IsCompeteDead() )
				SetCompeteDead( false );

			LOG::Log11N4S( LM_CHARACTER_RESURRECTION, GetAccountID(), GetSID(), CRT_FAIRY_POTION, 0, 0, 0, GetX(), GetY(), GetLayer(), nRecoveryEXP, GetEXP(),
				GetAccountName(), LOG::STR_NTS, GetName(), LOG::STR_NTS, "", LOG::STR_NTS, "" , LOG::STR_NTS );

			return true;
		}
	}

	return false;
}

void StructPlayer::SetPCBangMode( char nPCBangMode )
{
	if( !IsLoginComplete() || !IsInWorld() )
		return;

	if( m_nPCBangMode == nPCBangMode )
		return;

	switch( m_nPCBangMode )
	{
	case GameRule::PCBANG_ALLY_BONUS:
		{
			RemoveState( StructState::PCBANG_MEMBER_SHIP, 1 );
		}
		break;
	case GameRule::PCBANG_PREMIUM_BONUS:
		{
			RemoveState( StructState::PCBANG_PREMIUM_MEMBER_SHIP, 1 );

			// RemoveState에 의해 m_vStateList가 메모리상에서 이동될 수 있으므로 위에서 미리 검색해두면 안 됨
			StructState *pState = GetState( StructState::STAMINA_SAVE );
			if( pState && pState->IsHolded() && ( !IsGameTimeLimited() || GetContinuousPlayTime() < GameRule::nMaxHealthyGameTime ) )
			{
				pState->ReleaseRemainDuration();
				onUpdateState( *pState );
			}
		}
		break;
	default:
		break;
	}

	m_nPCBangMode = nPCBangMode;

	switch( nPCBangMode )
	{
	case GameRule::PCBANG_ALLY_BONUS:
		{
			// PC방 버프는 StateValue로 경험치 배율이 들어간다. (클라 메시지 표시용, 2013-12-24)
			AddState( StructState::PCBANG_MEMBER_SHIP, 0, 1, GetArTime(), -1, true, GameRule::fAllyPCBangBonusRate * 100 );
		}
		break;
	case GameRule::PCBANG_PREMIUM_BONUS:
		{
			// PC방 버프는 StateValue로 경험치 배율이 들어간다. (클라 메시지 표시용, 2013-12-24)
			AddState( StructState::PCBANG_PREMIUM_MEMBER_SHIP, 0, 1, GetArTime(), -1, true, GameRule::fPremiumPCBangBonusRate * 100 );

			// AddState에 의해 m_vStateList가 메모리상에서 이동될 수 있으므로 위에서 미리 검색해두면 안 됨
			StructState *pState = GetState( StructState::STAMINA_SAVE );
			if( pState && !GameRule::bApplyStaminaBonusInPremiumPCBang )
			{
				pState->HoldRemainDuration();
				onUpdateState( *pState );
			}
		}
		break;
	default:
		break;
	}

	m_TitleManager.UpdateTitleConditionByPCBangMode( m_nPCBangMode );
}

char StructPlayer::GetPCBangMode()
{
	return ENV().GetInt( "game.pcbang_bonus_server" ) ? m_nPCBangMode : 0;
}

bool StructPlayer::IsGameTimeLimited()
{
	return GameRule::bLimitGameTime && GetAge() <= GameRule::nMaxGameTimeLimitedAge;
}

void StructPlayer::sendBonusExpJpMsg()
{
	char szTmp[1024];

	TS_SC_BONUS_EXP_JP *pMsg = reinterpret_cast< TS_SC_BONUS_EXP_JP * >(szTmp);
	TS_SC_BONUS_EXP_JP::BONUS_INFO *pBonus = reinterpret_cast< TS_SC_BONUS_EXP_JP::BONUS_INFO * >(pMsg + 1);
	int remain_buf_len = sizeof( szTmp ) - sizeof( TS_SC_BONUS_EXP_JP );

	// 주의: char 배열을 버퍼로 사용할 때는 TS_MESSAGE의 생성자가 호출되지 않으므로 id, size를 직접 세팅해야 한다.
	pMsg->id = TM_SC_BONUS_EXP_JP;
	pMsg->handle = GetHandle();
	pMsg->count = 0;
	pMsg->size = sizeof(TS_SC_BONUS_EXP_JP);

	for( int i = 0 ; i < MAX_BONUS_TYPE ; i++ )
	{
		if( m_pBonusInfo[i].exp != -1 )
		{
			s_memcpy( pBonus + pMsg->count, remain_buf_len, &m_pBonusInfo[ i ], sizeof( m_pBonusInfo[ i ] ) );
			remain_buf_len -= sizeof( m_pBonusInfo[ i ] );

			++pMsg->count;
		}
	}

	if( !pMsg->count )
		return;

	pMsg->size += sizeof( TS_SC_BONUS_EXP_JP::BONUS_INFO ) * pMsg->count;

	PendStream( this, pMsg, pMsg->size );

	clearPendingBonusMsg();
}

void StructPlayer::AddFriend( const char * szName )
{
	DBQuery( new DB_InsertFriend( szName, false, this ) );
}

bool StructPlayer::addFriend( const char * szName )
{
	THREAD_SYNCRONIZE( s_FriendsLock );

	for( std::vector< std::string >::iterator it = m_vFriend.begin(); it != m_vFriend.end(); ++it )
	{
		if( !_stricmp( (*it).c_str(), szName ) )
			return false;
	}

	m_vFriend.push_back( szName );

	return true;
}

void StructPlayer::AddFriendOf( const char * szName )
{
	THREAD_SYNCRONIZE( s_FriendsLock );

	for( std::vector< std::string >::iterator it = m_vFriendOf.begin(); it != m_vFriendOf.end(); ++it )
	{
		if( !_stricmp( (*it).c_str(), szName ) )
			return;
	}

	m_vFriendOf.push_back( szName );
}

void StructPlayer::AddDenial( const char * szName )
{
	DBQuery( new DB_InsertFriend( szName, true, this ) );
}

bool StructPlayer::addDenial( const char *szName )
{
	THREAD_SYNCRONIZE( s_FriendsLock );

	for( std::vector< std::string >::iterator it = m_vDenial.begin(); it != m_vDenial.end(); ++it )
	{
		if( !_stricmp( (*it).c_str(), szName ) )
			return false;
	}

	m_vDenial.push_back( szName );

	return true;
}

void StructPlayer::AddDenialOf( const char * szName )
{
	THREAD_SYNCRONIZE( s_FriendsLock );

	for( std::vector< std::string >::iterator it = m_vDenialOf.begin(); it != m_vDenialOf.end(); ++it )
	{
		if( !_stricmp( (*it).c_str(), szName ) )
			return;
	}

	m_vDenialOf.push_back( szName );
}

void StructPlayer::DelFriend( const char * szName )
{
	DBQuery( new DB_DeleteFriend( GetName(), szName, false, this ) );
}

bool StructPlayer::delFriend( const char * szName )
{
	THREAD_SYNCRONIZE( s_FriendsLock );

	for( std::vector< std::string >::iterator it = m_vFriend.begin(); it != m_vFriend.end(); ++it )
	{
		if( !_stricmp( (*it).c_str(), szName ) )
		{
			m_vFriend.erase( it );
			return true;
		}
	}

	return false;
}

void StructPlayer::DelFriendOf( const char * szName )
{
	THREAD_SYNCRONIZE( s_FriendsLock );

	for( std::vector< std::string >::iterator it = m_vFriendOf.begin(); it != m_vFriendOf.end(); ++it )
	{
		if( !_stricmp( (*it).c_str(), szName ) )
		{
			m_vFriendOf.erase( it );
			return;
		}
	}
}

void StructPlayer::DelDenial( const char * szName )
{
	DBQuery( new DB_DeleteFriend( GetName(), szName, true, this ) );
}

bool StructPlayer::delDenial( const char * szName )
{
	THREAD_SYNCRONIZE( s_FriendsLock );

	for( std::vector< std::string >::iterator it = m_vDenial.begin(); it != m_vDenial.end(); ++it )
	{
		if( !_stricmp( (*it).c_str(), szName ) )
		{
			m_vDenial.erase( it );
			return true;
		}
	}

	return false;
}

void StructPlayer::DelDenialOf( const char * szName )
{
	THREAD_SYNCRONIZE( s_FriendsLock );

	for( std::vector< std::string >::iterator it = m_vDenialOf.begin(); it != m_vDenialOf.end(); ++it )
	{
		if( !_stricmp( (*it).c_str(), szName ) )
		{
			m_vDenialOf.erase( it );
			return;
		}
	}
}

bool StructPlayer::IsFriend( const char * szName )
{
	THREAD_SYNCRONIZE( s_FriendsLock );

	for( std::vector< std::string >::iterator it = m_vFriend.begin(); it != m_vFriend.end(); ++it )
	{
		if( !_stricmp( (*it).c_str(), szName ) )
		{
			return true;
		}
	}

	return false;
}

bool StructPlayer::IsFriendOf( const char * szName )
{
	THREAD_SYNCRONIZE( s_FriendsLock );

	for( std::vector< std::string >::iterator it = m_vFriendOf.begin(); it != m_vFriendOf.end(); ++it )
	{
		if( !_stricmp( (*it).c_str(), szName ) )
		{
			return true;
		}
	}

	return false;
}

bool StructPlayer::IsDenial( const char * szName )
{
	THREAD_SYNCRONIZE( s_FriendsLock );

	for( std::vector< std::string >::iterator it = m_vDenial.begin(); it != m_vDenial.end(); ++it )
	{
		if( !_stricmp( (*it).c_str(), szName ) )
		{
			return true;
		}
	}

	return false;
}

bool StructPlayer::IsDenialOf( const char * szName )
{
	THREAD_SYNCRONIZE( s_FriendsLock );

	for( std::vector< std::string >::iterator it = m_vDenialOf.begin(); it != m_vDenialOf.end(); ++it )
	{
		if( !_stricmp( (*it).c_str(), szName ) )
		{
			return true;
		}
	}

	return false;
}

const size_t StructPlayer::DoEachFriend( FriendDenialFunctor & fo )
{
	THREAD_SYNCRONIZE( s_FriendsLock );

	size_t nCount = 0;

	for( std::vector< std::string >::iterator it = m_vFriend.begin() ; it != m_vFriend.end() ; ++it )
	{
		if( fo( (*it) ) )
			++nCount;
	}

	return nCount;
}

const size_t StructPlayer::DoEachFriendOf( FriendDenialFunctor & fo )
{
	THREAD_SYNCRONIZE( s_FriendsLock );

	size_t nCount = 0;

	for( std::vector< std::string >::iterator it = m_vFriendOf.begin() ; it != m_vFriendOf.end() ; ++it )
	{
		if( fo( (*it) ) )
			++nCount;
	}

	return nCount;
}

const size_t StructPlayer::DoEachDenial( FriendDenialFunctor & fo )
{
	THREAD_SYNCRONIZE( s_FriendsLock );

	size_t nCount = 0;

	for( std::vector< std::string >::iterator it = m_vDenial.begin() ; it != m_vDenial.end() ; ++it )
	{
		if( fo( (*it) ) )
			++nCount;
	}

	return nCount;
}

const size_t StructPlayer::DoEachDenialOf( FriendDenialFunctor & fo )
{
	THREAD_SYNCRONIZE( s_FriendsLock );

	size_t nCount = 0;

	for( std::vector< std::string >::iterator it = m_vDenialOf.begin() ; it != m_vDenialOf.end() ; ++it )
	{
		if( fo( (*it) ) )
			++nCount;
	}

	return nCount;
}

const bool StructPlayer::IsInStartedCompete( const bool includeCounting ) const
{
	return GetCompeteID() && CompeteManager::Instance().IsStarted( GetCompeteID(), includeCounting );
}

const bool StructPlayer::ResurrectByCompete()
{
	AddHP( GameRule::COMPETE_RESURRECT_HP_RECOVER_RATE * GetMaxHP() );

	ClearRemovedStateByDead();

	SetCompeteDead( false );

	Save();

	BroadcastHPMPMsg( this, GetHP(), 0 );

	LOG::Log11N4S( LM_CHARACTER_RESURRECTION, GetAccountID(), GetSID(), CRT_COMPETE, 0, 0, 0, GetX(), GetY(), GetLayer(), 0, GetEXP(),
		GetAccountName(), LOG::STR_NTS, GetName(), LOG::STR_NTS, "", LOG::STR_NTS, "" , LOG::STR_NTS );

	return true;
}

const bool StructPlayer::ResurrectByDeathmatch()
{
	int instance_game_type = GameContent::GetDeathmatchType( GetPos() );

	AddHP( GetMaxHP() );
	AddMP( GetMaxMP() );

	ClearRemovedStateByDead();

	Save();

	BroadcastHPMPMsg( this, GetHP(), 0 );

	PendWarpToDeathmatch( instance_game_type );

	LOG::Log11N4S( LM_CHARACTER_RESURRECTION, GetAccountID(), GetSID(), CRT_COMPETE, 0, 0, 0, GetX(), GetY(), GetLayer(), 0, GetEXP(),
		GetAccountName(), LOG::STR_NTS, GetName(), LOG::STR_NTS, "", LOG::STR_NTS, "" , LOG::STR_NTS );

	return true;
}

void StructPlayer::IncBattleArenaPenalty()
{
	time_t tCurrent = time( NULL );

	++m_nArenaPenaltyCount;
	m_tArenaBlock = tCurrent + GameRule::GetBattleArenaBlockDuration( m_nArenaPenaltyCount );
	m_tArenaPenaltyDecrease = tCurrent + GameRule::GetBattleArenaPenaltyDuration( m_nArenaPenaltyCount );
}

void StructPlayer::RequestSecurityNo( int nMode )
{
	TS_SC_REQUEST_SECURITY_NO msg;

	msg.mode = nMode;

	PendMessage( this, &msg );
}