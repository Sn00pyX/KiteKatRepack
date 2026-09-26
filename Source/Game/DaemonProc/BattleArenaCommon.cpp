
#include <toolkit/XStringUtil.h>

#include "LogClient/LogClient.h"

#include "BattleArenaCommon.h"
#include "StructSummon.h"
#include "StructPlayer.h"
#include "ChannelManager.h"
#include "LuaVM.h"
#include "ThreadPlayerHelper.h"


const char *	FLAG_BATTLE_ARENA_EXERCISE_GAME_READY_STATE	= "ExerciseReadyState";
const char *	FLAG_BATTLE_ARENA_PROP_STATE				= "PropState";


BattleArenaQuitFunctor::BattleArenaQuitFunctor( _ARENA_LEAVE_TYPE eLeaveType,
											   const BattleArenaBaseServer* pArenaBase,
											   unsigned char nInstanceNo )
	: m_eLeaveType( eLeaveType )
	, m_pArenaBase( pArenaBase )
	, m_nInstanceNo( nInstanceNo )
{
}

void	BattleArenaQuitFunctor::operator()( ArObject* pObj )	const
{
	StructPlayer* pPlayer = static_cast< StructPlayer* >( pObj );

	// _removePlayer나 _wrapUpInstance에 의해 경기에서 이탈된 상태의 유저만 QuitFuctor에 의해 외부로 워프되어야 함
	assert( !pPlayer->GetBattleArenaID() && !pPlayer->GetBattleArenaInstanceNo() );

	// 아레나 내부에 있지 않다면 아무것도 안 함
	// * 외부에 있는데 내부로 PendWarp 된 상태이거나 BattleArenaManager::onProcess에서 vPendedEnterPlayerList에 포함되어 있지만 락은 풀린 시점이어서
	//   곧 PendWarp 될 상태일 수도 있는데, vPendedEnterPlayerList 포함된 상태라면 이미 경기에서 이탈된 상태기 때문에
	//   다시 락을 걸고 PendWarp 하기 직전에 검사하는 과정에서 탈락될 걸로 예상.
	//   그리고 PendWarp까지 됐지만 아직 ProcessWarp가 호출되지 않은 경우에는 ProcessWarp 안에서 경기에 참여 상태인지 워프 처리 직전에 검사.
	if( !pPlayer->IsInBattleArena() )
		return;

	// 일단 사망 상태면 부활시켜 줌(안 해주면 밖에서 부활하며 온갖 페널티를 먹거나 대모요정의 병을 날림)
	// 아레나 내부에서 부활 시에는 모든 지속효과를 복구하지만 이 시점에서는 복구를 해주더라도 퇴장 처리에서 결국 모두 삭제되기 때문에 복구해주지 않는다.
	pPlayer->Resurrect( CRT_BATTLE_ARENA, pPlayer->GetMaxHP(), pPlayer->GetMaxMP(), 0, false );

	// 이탈 스크립트 실행(추가로 접속 종료에 의한 것이 아니라는 파라미터를 세팅해 줌)
	std::vector< std::string > vTagReplacement;
	vTagReplacement.push_back( "#@disconnect@#" );
	vTagReplacement.push_back( "0" );

	ThreadPlayerHelper TPHelper( pPlayer );
	BattleArenaRunEventScript( m_pArenaBase, BASET_LEAVE_INSTANCE, m_nInstanceNo, pPlayer, &vTagReplacement );

	// 지속효과 제거
	pPlayer->RemoveAllStateByQuittingBattleArena();

	if( pPlayer->GetMainSummon() )
		pPlayer->GetMainSummon()->RemoveAllStateByQuittingBattleArena();
	if( pPlayer->GetSubSummon() )
		pPlayer->GetSubSummon()->RemoveAllStateByQuittingBattleArena();

	ArPosition posExit;
	pPlayer->GetPositionOnEnterInstanceGame( &posExit );

	pPlayer->RestoreStatesOnLeaveInstanceGame( false );

	unsigned char nLayer = 0;

	// 수련자의 섬 같이 인원 제한 채널에 레이어 설정된 구역으로 워프해야 하는 경우 적당한 레이어 얻기
	int nTargetChannel = ChannelManager::GetChannelId( posExit.GetX(), posExit.GetY() );
	if( nTargetChannel && ChannelManager::GetChannelType( nTargetChannel ) == ChannelManager::TYPE_USER_LIMIT )
	{
		nLayer = ChannelManager::GetProperLayer( posExit.GetX(), posExit.GetY() );
	}

	// PendWarp는 지역락 걸고 해야되는데, 여긴 이미 csBattle이 걸려있어서 지역락을 유저마다 따로 걸어줄 수는 없음
	// 근데 애초에 여기 들어올 때 LockWholeInstance나 LockWholeArena를 하고 들어올 거고,
	// 모든 유저는 경기장 안에 있을 거라서 따로 지역락은 걸지 않아도 문제는 되지 않을 걸로 예상됨
	pPlayer->PendWarp( posExit.GetX(), posExit.GetY(), nLayer );
	pPlayer->SetMove( pPlayer->GetCurrentPosition( GetArTime() ), 0 );
	ArcadiaServer::Instance().SetObjectPriority( pPlayer, ArSchedulerObject::UPDATE_PRIORITY_HIGHEST );
}


bool	IsValidBattleArenaInstanceNo( unsigned char nInstanceNo )
{
	return nInstanceNo > 0 && nInstanceNo <= MAX_BATTLE_ARENA_INSTANCE_NO_PER_ARENA;
}

// 레벨에 따른 배틀 아레나 구분 등급 번호 얻기(유효하지 않은 레벨에 대해서는 -1을 반환, 그 외에는 0 ~ 2 사이의 값을 반환)
_BATTLE_GRADE	GetBattleArenaGrade( int nLevel )
{
	// 최소 레벨 미만
	if( nLevel < GameRule::GetItemLevelLimitByRank( 3 ) )
		return BG_INVALID;
	// 최저 등급
	if( nLevel < GameRule::GetItemLevelLimitByRank( 5 ) )
		return BG_ROOKIE;
	// 중간 등급
	if( nLevel < GameRule::GetItemLevelLimitByRank( 7 ) )
		return BG_GROW;
	// 최고 등급
	return BG_MAJOR;
}

bool	IsValidBattleArenaGrade( _BATTLE_GRADE eGrade )
{
	if( eGrade == BG_ROOKIE || eGrade == BG_GROW || eGrade == BG_MAJOR )
	{
		return true;
	}

	return false;
}


// 직접 호출해서 사용하지 말 것. 이 함수 대신 BattleArena::LockWholeArena, BattleArenaInstance::LockWholeInstance 함수를 사용할 것.
ArcadiaLock	_LockWholeBattleArena( const BattleArenaBaseServer* pArenaBase, unsigned short nLayer )
{
	return ArcadiaServer::Instance().LockArea( pArenaBase->nArenaRegionLeft, pArenaBase->nArenaRegionTop,
		pArenaBase->nArenaRegionRight, pArenaBase->nArenaRegionBottom, nLayer );
}


// 아레나 이벤트별 스크립트에 대한 태그 처리 결과 반환
std::string	GetBattleArenaEventScript( const BattleArenaBaseServer* pArenaBase,
									  _BATTLE_ARENA_SCRIPT_EXEC_TYPE eExecType,
									  unsigned char nInstanceNo,
									  const std::vector< std::string >* pvTagReplacement )
{
	const char* pszScript = NULL;
	switch( eExecType )
	{
	case BASET_CREATE_INSTANCE:		pszScript = pArenaBase->strScriptOnCreate.c_str();	break;
	case BASET_ENTER_INSTANCE:		pszScript = pArenaBase->strScriptOnEnter.c_str();	break;
	case BASET_START_INSTANCE:		pszScript = pArenaBase->strScriptOnStart.c_str();	break;
	case BASET_LEAVE_INSTANCE:		pszScript = pArenaBase->strScriptOnLeave.c_str();	break;
	case BASET_END_INSTANCE:		pszScript = pArenaBase->strScriptOnEnd.c_str();		break;
	case BASET_DESTROY_INSTANCE:	pszScript = pArenaBase->strScriptOnDestroy.c_str();	break;
	default:
		assert( 0 );
		return "";
	}

	// 스크립트가 없거나(이런 경우는 없지만) 길이가 0이면(첫 번째 문자가 NULL) 그냥 리턴
	if( !pszScript || !( *pszScript ) )
		return "";

	std::string strScriptBuffer( pszScript );

	// 레이어 번호 태그 처리
	XStringUtil::ReplaceFormat( strScriptBuffer, "#@instance_no@#", "%d", (int)nInstanceNo );

	// 그 외에 추가 태그 처리
	if( pvTagReplacement )
	{
		size_t nTagCount = pvTagReplacement->size() / 2;
		for( int nTagIndex = 0 ; nTagIndex < nTagCount ; ++nTagIndex )
		{
			const std::string& strTag = (*pvTagReplacement)[ nTagIndex * 2 ];
			const std::string& strReplacement = (*pvTagReplacement)[ nTagIndex * 2 + 1 ];

			XStringUtil::Replace( strScriptBuffer, strTag.c_str(), strReplacement.c_str() );
		}
	}

	return strScriptBuffer;
}

void	BattleArenaRunScriptWithLog( const BattleArenaBaseServer* pArenaBase,
									_BATTLE_ARENA_SCRIPT_EXEC_TYPE eExecType,
									unsigned char nInstanceNo,
									const std::string& strScript,
									const StructPlayer* pExecutor )
{
	// 경기장 영역 내 혹은 스크립트 호출자 주변으로 지역락이 반드시 걸려 있어야 함
	// * 유저간 상호작용은 csBattle 때문에 별 문제 없지만 스크립트가 뭘 할지는 모르는 거니까.
	assert( ( pExecutor && ArcadiaServer::Instance().IsLocked( pExecutor ) ) ||
		( !pExecutor && ArcadiaServer::Instance().IsLocked(
							pArenaBase->nArenaRegionLeft,
							pArenaBase->nArenaRegionTop,
							pArenaBase->nArenaRegionRight,
							pArenaBase->nArenaRegionBottom,
							nInstanceNo ) ) );

	LUA()->RunString( strScript.c_str() );

	LOG::Log11N4S( LM_BATTLE_ARENA_PROCESS,
		( pExecutor ) ? pExecutor->GetAccountID() : 0, ( pExecutor ) ? pExecutor->GetPlayerUID() : 0,
		pArenaBase->nID, nInstanceNo,
		0, 0, 0, 0, 0, 0,
		eExecType,
		( pExecutor ) ? pExecutor->GetAccountName() : "", LOG::STR_NTS,
		( pExecutor ) ? pExecutor->GetAlias() : "", LOG::STR_NTS,
		strScript.c_str(), LOG::STR_NTS,
 ( pExecutor ) ? pExecutor->GetAlias() : "", LOG::STR_NTS );
}

// 각 상황에 맞는 지역락 -> csArena -> csBattle을 걸고 호출해야 함
// * 스크립트 안에서 호출하는 함수가 BattleArenaManager의 멤버 함수를 호출하는 경우 csArena/csBattle을 중첩으로 걸 수도 있으나,
//   CRITICAL_SECTION의 특성에 따라 중첩해서 거는 것이 허용됨.
//   단, csArena/csBattle을 걸고 BattleArenaRunEventScript를 호출하는 경우에는 반드시 현재 걸려있는 csArena/csBattle만 걸어야 함.
//   csArena/csBattle은 실제로 1개가 아니고 각 아레나별/인스턴스별로 여러 개가 생성되어 있기 때문에
//   BattleArenaRunEventScript 함수를 호출하는 시점에 걸려있던 csArena/csBattle과 이름은 같지만 다른 다른 인스턴스를
//   스크립트 안에서 걸게 되면 데드락을 유발하게 됨(이름은 같지만 서로 다른 락을 규칙성 없이 교차해서 걸게 되므로)
void	BattleArenaRunEventScript( const BattleArenaBaseServer* pArenaBase,
								  _BATTLE_ARENA_SCRIPT_EXEC_TYPE eExecType,
								  unsigned char nInstanceNo,
								  const StructPlayer * pExecutor,
								  const std::vector< std::string >* pvTagReplacement )
{
	std::string& strEventScript = GetBattleArenaEventScript( pArenaBase, eExecType, nInstanceNo, pvTagReplacement );

	BattleArenaRunScriptWithLog( pArenaBase, eExecType, nInstanceNo, strEventScript, pExecutor );
}

