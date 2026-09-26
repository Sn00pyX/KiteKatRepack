
#include <mmo/ArcadiaServer.h>
#include <logging/FileLog.h>
#include <toolkit/XEnv.h>
#include <toolkit/XConsole.h>

#include "LogClient/LogClient.h"

#include "RankingManager.h"
#include "StructPlayer.h"
#include "DB_Commands.h"


const size_t		RankingManager::RANKING_TOP_RECORD_MIN_RANK[ RANKING_TYPE_MAX_INDEX ]		= {	10,		0,	10,	10,	10 };
const c_fixed10		RankingManager::RANKING_TOP_RECORD_MIN_SCORE[ RANKING_TYPE_MAX_INDEX ]		= {	100,	0,	0,	0,	10 };

const unsigned int RankingManager::_RANKING_INFO::GetRank( const int nPlayerUID ) const
{
	_RANKING_DATA * pRank = NULL;
	if( m_hsRanking.lookup( nPlayerUID, pRank ) && pRank->m_bIsValid )
		return pRank->m_nRank;

	return 0;
}

const c_fixed10 RankingManager::_RANKING_INFO::GetRankingScore( const int nPlayerUID ) const
{
	_RANKING_DATA * pRank = NULL;
	if( m_hsRanking.lookup( nPlayerUID, pRank ) && pRank->m_bIsValid )
		return pRank->m_fScore;

	return 0;
}

void RankingManager::_RANKING_INFO::SetRankingScore( const int nPlayerUID, const char * pszPlayerName, const c_fixed10 & fScore, const bool bIsValid )
{
	_RANKING_DATA * pRank = NULL;

	if( m_hsRanking.lookup( nPlayerUID, pRank ) )
	{
		if( pRank->m_fScore == fScore )
			return;

		bool bIsScoreUp = pRank->m_fScore < fScore;
		pRank->m_fScore = fScore;
		// pRank->m_bIsValid 는 RankingManager::InvalidateRankingScore에 의해서만 변경됨

		rerankData( pRank, bIsScoreUp );
	}
	else
	{
		StructPlayer::RegisterPlayerName( nPlayerUID, pszPlayerName );

		if( fScore == 0 )
			return;

		pRank = new _RANKING_DATA( nPlayerUID, fScore, bIsValid );
		pRank->m_nRank = static_cast< int >( m_vRanking.size() + 1 );

		m_vRanking.push_back( pRank );
		m_hsRanking.add( nPlayerUID, pRank );

		// 없던 데이터 추가한 경우에는 최하위 랭크로 책정되어 있으니 상향 체크를 진행
		rerankData( pRank, true );
	}
}

void RankingManager::_RANKING_INFO::ClearRankingData()
{
	for( std::vector< _RANKING_DATA * >::const_iterator it = m_vRanking.begin() ; it != m_vRanking.end() ; ++it )
		delete (*it);

	m_vRanking.clear();
	m_hsRanking.clear();
}

void RankingManager::_RANKING_INFO::rerankData( RankingManager::_RANKING_DATA * pRank, const bool bIsScoreUp )
{
	std::vector< _RANKING_DATA * >::iterator it = m_vRanking.begin() + ( pRank->m_nRank - 1 );

#ifdef _DEBUG
	assert( it == std::find( m_vRanking.begin(), m_vRanking.end(), pRank ) );
#endif

	if( it == m_vRanking.end() )
	{
		assert( 0 );
		return;
	}

	if( bIsScoreUp )
	{
		if( it == m_vRanking.begin() )
			return;

		std::vector< _RANKING_DATA * >::iterator itBetter = it - 1;
		while( (*itBetter)->m_fScore < (*it)->m_fScore )
		{
			_RANKING_DATA * pSwapBuffer = (*itBetter);
			(*itBetter) = (*it);
			(*it) = pSwapBuffer;

			++( (*it)->m_nRank );
			--( (*itBetter)->m_nRank );

			if( itBetter == m_vRanking.begin() )
				break;

			--it;
			--itBetter;
		}
	}
	else
	{
		if( it + 1 == m_vRanking.end() )
			return;

		std::vector< _RANKING_DATA * >::iterator itWorse = it + 1;
		while( (*itWorse)->m_fScore > (*it)->m_fScore )
		{
			_RANKING_DATA * pSwapBuffer = (*itWorse);
			(*itWorse) = (*it);
			(*it) = pSwapBuffer;

			--( (*it)->m_nRank );
			++( (*itWorse)->m_nRank );

			if( ++itWorse == m_vRanking.end() )
				break;

			++it;
		}
	}
}

RankingManager & RankingManager::Instance()
{
	static RankingManager _instance;

	return _instance;
}

RankingManager::~RankingManager()
{
	if( IsInitialized() )
		DeInit();
}

void RankingManager::Init()
{
	m_bInitialized = true;
	ArcadiaServer::Instance().SetObjectPriority( &RankingManager::Instance(), ArSchedulerObject::UPDATE_PRIORITY_LOW );
}

void RankingManager::DeInit()
{
	ArcadiaServer::Instance().SetObjectPriority( this, ArSchedulerObject::UPDATE_PRIORITY_IDLE );
	m_bInitialized = false;

	for( int i = 0 ; i < RANKING_TYPE_MAX_INDEX ; ++i )
	{
		_RANKING_INFO * pRankingInfo = getRankingInfo( static_cast< _RANKING_TYPE >( i ) );

		if( !pRankingInfo )
			continue;

		THREAD_SYNCHRONIZE( pRankingInfo->m_csRanking );

		pRankingInfo->ClearRankingData();
	}
}

const bool RankingManager::IsInitialized() const
{
	return GetFinalPriority() != ArSchedulerObject::UPDATE_PRIORITY_IDLE;
}

void RankingManager::Push( GameDBManager::DBProc* pWork )
{
	THREAD_SYNCRONIZE( m_QueryLock );

	if( m_lQueryList.empty() )
	{
		DB().Push( pWork );
	}

	m_lQueryList.push_back( pWork );
}

void RankingManager::onEndQuery()
{
	THREAD_SYNCRONIZE( m_QueryLock );

	m_lQueryList.pop_front();

	if( !m_lQueryList.empty() )
	{
		DB().Push( m_lQueryList.front() );
	}
}

const time_t RankingManager::GetNextSettlingTime( const _RANKING_TYPE & eType ) const
{
	if( !IsInitialized() )
	{
		assert( 0 );
		return 0;
	}

	const _RANKING_INFO * pRankingInfo = getRankingInfo( eType );
	if( !pRankingInfo )
		return 0;

	return pRankingInfo->m_tNextSettling;
}

const bool RankingManager::SetNextSettlingTime( const RankingManager::_RANKING_TYPE & eType, const time_t & tNextSettlingTime )
{
	if( IsInitialized() )
	{
		assert( 0 );
		return false;
	}

	_RANKING_INFO * pRankingInfo = getRankingInfo( eType );
	if( !pRankingInfo )
		return false;

	pRankingInfo->m_tNextSettling = tNextSettlingTime;
	return true;
}

const unsigned int RankingManager::GetRank( const _RANKING_TYPE & eType, const int nPlayerUID ) const
{
	const _RANKING_INFO * pRankingInfo = getRankingInfo( eType );
	if( !pRankingInfo )
		return 0;

	THREAD_SYNCHRONIZE( pRankingInfo->m_csRanking );

	return pRankingInfo->GetRank( nPlayerUID );
}

const c_fixed10 RankingManager::GetRankingScore( const _RANKING_TYPE & eType, const int nPlayerUID ) const
{
	const _RANKING_INFO * pRankingInfo = getRankingInfo( eType );
	if( !pRankingInfo )
		return 0;

	THREAD_SYNCHRONIZE( pRankingInfo->m_csRanking );

	return pRankingInfo->GetRankingScore( nPlayerUID );
}

void RankingManager::SetRankingScore( const _RANKING_TYPE & eType, const int nPlayerUID, const char * pszPlayerName, const c_fixed10 & fScore, const bool bIsValid )
{
	_RANKING_INFO * pRankingInfo = getRankingInfo( eType );
	if( !pRankingInfo )
		return;

	THREAD_SYNCHRONIZE( pRankingInfo->m_csRanking );

	pRankingInfo->SetRankingScore( nPlayerUID, pszPlayerName, fScore, bIsValid );

	// 초기화된 상태에서 발생하는 SetRankingScore만 DB에 적용해야 하며, 초기화되지 않은 상태에서의 호출은 로딩에 의한 데이터 입력이므로 DB갱신 안 함
	if( IsInitialized() )
		Push( new DB_SetRankingScore( eType, nPlayerUID, fScore ) );
}

void RankingManager::AddRankingScore( const _RANKING_TYPE & eType, const int nPlayerUID, const char * pszPlayerName, const c_fixed10 & fAddend, const bool bIsValid )
{
	_RANKING_INFO * pRankingInfo = getRankingInfo( eType );
	if( !pRankingInfo )
		return;

	THREAD_SYNCHRONIZE( pRankingInfo->m_csRanking );

	c_fixed10 fScore = pRankingInfo->GetRankingScore( nPlayerUID );

	pRankingInfo->SetRankingScore( nPlayerUID, pszPlayerName, fScore + fAddend, bIsValid );

	// 초기화된 상태에서 발생하는 SetRankingScore만 DB에 적용해야 하며, 초기화되지 않은 상태에서의 호출은 로딩에 의한 데이터 입력이므로 DB갱신 안 함
	if( IsInitialized() )
		Push( new DB_SetRankingScore( eType, nPlayerUID, fScore + fAddend ) );
}

void RankingManager::DeleteRankingScore( const int nPlayerUID )
{
	for( int nRankingType = RANKING_TYPE_DONATION ; nRankingType < RANKING_TYPE_MAX_INDEX ; ++nRankingType )
	{
		_RANKING_TYPE eType = static_cast< _RANKING_TYPE >( nRankingType );

		_RANKING_INFO * pRankingInfo = getRankingInfo( eType );
		if( !pRankingInfo )
			continue;

		switch( eType )
		{
		case RANKING_TYPE_DONATION:
		case RANKING_TYPE_HUNTAHOLIC_THIS_MONTH:
		case RANKING_TYPE_HUNTAHOLIC_LAST_MONTH:
		case RANKING_TYPE_HUNTAHOLIC_TOTAL:
			{
				THREAD_SYNCHRONIZE( pRankingInfo->m_csRanking );

				_RANKING_DATA * pRank = NULL;
				if( !pRankingInfo->m_hsRanking.lookup( nPlayerUID, pRank ) )
					break;

				pRankingInfo->m_hsRanking.erase( nPlayerUID );

				std::vector< _RANKING_DATA * >::iterator it = pRankingInfo->m_vRanking.begin() + ( pRank->m_nRank - 1 );
				assert( (*it) == pRank );

				it = pRankingInfo->m_vRanking.erase( it );
				while( it != pRankingInfo->m_vRanking.end() )
				{
					--(*it)->m_nRank;
					++it;
				}

				// DB에서 삭제는 결산 때 일괄적으로 하는 것만 하고 이런 경우에는 점수를 0을 만들어 둠. 서버 재시작 시 로딩에서는 0점짜리는 메모리에 로드하지 않음.
				Push( new DB_SetRankingScore( eType, nPlayerUID, 0 ) );

				delete pRank;
			}
			break;
		case RANKING_TYPE_DONATION_REWARD:
			// 캐릭터 삭제한 경우에 처리되는 부분이므로 무효화시켜도 상관 없지만 삭제 캐릭터 복구받은 후에 보상을 못 받게 되는 경우가 발생하므로 여기선 아무것도 안 함.
			break;

		default:
			assert( 0 );
			break;
		}
	}
}

void RankingManager::GetRankingTopRecord( const _RANKING_TYPE & eType, std::vector< RankingRecord > & vTopRecord )
{
	_RANKING_INFO * pRankingInfo = getRankingInfo( eType );
	if( !pRankingInfo )
		return;

	THREAD_SYNCHRONIZE( pRankingInfo->m_csRanking );

	vTopRecord.clear();

	for( std::vector< _RANKING_DATA * >::const_iterator it = pRankingInfo->m_vRanking.begin() ; it != pRankingInfo->m_vRanking.end() ; ++it )
	{
		if( (*it)->m_nRank > RANKING_TOP_RECORD_MIN_RANK[ eType ] || (*it)->m_fScore < RANKING_TOP_RECORD_MIN_SCORE[ eType ] )
			break;

		vTopRecord.push_back( RankingRecord( (*it)->m_nPlayerUID, (*it)->m_fScore ) );
	}
}

const bool RankingManager::IsValidRankingScore( const _RANKING_TYPE & eType, const int nPlayerUID ) const
{
	const _RANKING_INFO * pRankingInfo = getRankingInfo( eType );
	if( !pRankingInfo )
		return false;

	THREAD_SYNCHRONIZE( pRankingInfo->m_csRanking );

	_RANKING_DATA * pRank = NULL;
	if( !pRankingInfo->m_hsRanking.lookup( nPlayerUID, pRank ) )
		return false;

	return pRank->m_bIsValid;
}

const bool RankingManager::InvalidateRankingScore( const _RANKING_TYPE & eType, const int nPlayerUID )
{
	_RANKING_INFO * pRankingInfo = getRankingInfo( eType );
	if( !pRankingInfo )
		return false;

	THREAD_SYNCHRONIZE( pRankingInfo->m_csRanking );

	_RANKING_DATA * pRank = NULL;
	if( !pRankingInfo->m_hsRanking.lookup( nPlayerUID, pRank ) )
		return false;

	if( !pRank->m_bIsValid )
		return false;

	// { 다음 달까지 10초 이하의 시간밖에 남아있지 않다면 무조건 실패 처리(10초 후에 달이 바뀌어 있는지 체크)
	// SCRIPT_ShowDonationMenu에서 InvalidateRankingScore 함수를 호출하는 시점과 그 이후 시간 관련 처리 시점의 시간차로 인해
	// 달이 바뀌는 순간 보상을 받으면 바뀌기 직전의 보상 대상자가 바뀐 이후의 한 달간 사용할 수 있는 보상 아이템을 받을 수 있기 때문
	time_t tCurrent = time( NULL );
	struct tm tmCurrent;
	errno_t nError = localtime_s( &tmCurrent, &tCurrent );
	if( nError )
	{
		assert( 0 );
		return false;
	}

	tCurrent += 10;
	struct tm tmPast10Sec;
	nError = localtime_s( &tmPast10Sec, &tCurrent );
	if( nError )
	{
		assert( 0 );
		return false;
	}

	if( tmCurrent.tm_mon != tmPast10Sec.tm_mon )
		return false;
	// } 다음 달까지 10초 이하의 시간밖에 남아있지 않다면 무조건 실패 처리(10초 후에 달이 바뀌어 있는지 체크)

	pRank->m_bIsValid = false;
	Push( new DB_InvalidateRankingScore( eType, nPlayerUID ) );

	return true;
}

void RankingManager::onProcess( int nThreadIdx )
{
	char buf[255];
	s_sprintf( buf, _countof( buf ), "thread.scheduler.%d.proc", nThreadIdx );
	ENV().Set( buf, "RankingManager" );

	extern __declspec( thread ) XSEH::THREAD_INFO s_ThreadInfo;
	s_sprintf( s_ThreadInfo.job_info, _countof( s_ThreadInfo.job_info ), "RankingManager(0x%08X)", (UINT_PTR)this );
	s_ThreadInfo.last_execute_time = GetArTime();

	time_t tCurrent = time( NULL );

	for( int nRankingType = RANKING_TYPE_DONATION ; nRankingType < RANKING_TYPE_MAX_INDEX ; ++nRankingType )
	{
		_RANKING_TYPE eType = static_cast< _RANKING_TYPE >( nRankingType );
		_RANKING_INFO * pRankingInfo = getRankingInfo( eType );
		if( !pRankingInfo )
			continue;

		THREAD_SYNCHRONIZE( pRankingInfo->m_csRanking );

		// 랭킹 결산 처리 시점이 지나지 않은 건 패스
		if( pRankingInfo->m_tNextSettling > tCurrent )
			continue;

		// 기부 랭킹 결산 처리 시 기부 랭킹 보상 결산 처리도 함께 진행되며, 기부 랭킹 보상 결산 처리 시는 다음 결산 시점 설정만 메모리에 세팅 됨(DB에는 미리 세팅됨)

		time_t tNextSettling = INFINITE;
		// 다음 결산 시점 설정
		switch( eType )
		{
		case RANKING_TYPE_DONATION:
		case RANKING_TYPE_DONATION_REWARD:
		case RANKING_TYPE_HUNTAHOLIC_THIS_MONTH:
		case RANKING_TYPE_HUNTAHOLIC_LAST_MONTH:
			{
				// 다음 달 1일 0시 0분 0초로 설정
				struct tm tmLocal;
				errno_t nError = localtime_s( &tmLocal, &tCurrent );
				assert( !nError );

				++tmLocal.tm_mon;
				tmLocal.tm_mday = 1;
				tmLocal.tm_hour = 0;
				tmLocal.tm_min = 0;
				tmLocal.tm_sec = 0;
				tmLocal.tm_isdst = -1;

				tNextSettling = mktime( &tmLocal );
				assert( tNextSettling != -1 );

				_cprint( "Settling a ranking data: id(%d), tCurrent(%I64d), tNextSettling(%I64d -> %I64d = %04d-%02d-%02d %02d:%02d:%02d)\n",
					eType, tCurrent, pRankingInfo->m_tNextSettling, tNextSettling,
					tmLocal.tm_year + 1900, tmLocal.tm_mon + 1, tmLocal.tm_mday, tmLocal.tm_hour, tmLocal.tm_min, tmLocal.tm_sec );
				FILELOG( "Settling a ranking data: id(%d), tCurrent(%I64d), tNextSettling(%I64d -> %I64d = %04d-%02d-%02d %02d:%02d:%02d)",
					eType, tCurrent, pRankingInfo->m_tNextSettling, tNextSettling,
					tmLocal.tm_year + 1900, tmLocal.tm_mon + 1, tmLocal.tm_mday, tmLocal.tm_hour, tmLocal.tm_min, tmLocal.tm_sec );

				// 새로 계산된 다음 랭킹 결산 시점이 열흘 이내일 경우 그 다음 달로 재 계산
				if( tNextSettling - tCurrent < 864000 )
				{
					if( ENV().GetInt( "game.generate_dump_on_ranking_error", -1 ) == eType )
					{
						try
						{
							// Exception 형태로 오류 메시지를 날리기 위한 버퍼
							char szErrorMsg[128];

							struct tm tmCurrent;
							if( localtime_s( &tmCurrent, &tCurrent ) )
								throw "Time value calculating failed";

							char szDumpFileName[ MAX_PATH ];
							s_sprintf( szDumpFileName, _countof( szDumpFileName ), "Arcadia %04d-%02d-%02d %02d-%02d-%02d.dmp",
								tmCurrent.tm_year + 1900, tmCurrent.tm_mon + 1, tmCurrent.tm_mday, tmCurrent.tm_hour, tmCurrent.tm_min, tmCurrent.tm_sec );

							HANDLE hDumpFile = CreateFile( szDumpFileName, GENERIC_WRITE, FILE_SHARE_READ, 0, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, 0 );
							if( hDumpFile == INVALID_HANDLE_VALUE )
							{
								DWORD nLastError = GetLastError();
								s_sprintf( szErrorMsg, _countof( szErrorMsg ), "CreateFile failed - %u", nLastError );
								throw szErrorMsg;
							}

							if( !MiniDumpWriteDump( GetCurrentProcess(), GetCurrentProcessId(), hDumpFile, MiniDumpWithFullMemory, NULL, NULL, NULL ) )
							{
								DWORD nLastError = GetLastError();
								s_sprintf( szErrorMsg, _countof( szErrorMsg ), "MiniDumpWriteDump failed - %u", nLastError );
								throw szErrorMsg;
							}

							CloseHandle( hDumpFile );
						}
						catch( const char * pszException )
						{
							_cprint( " -> Generating a dump failed(Engine: %s).\n", pszException );
							FILELOG( " -> Generating a dump failed(Engine: %s).", pszException );
						}
						catch( std::exception & e )
						{
							_cprint( " -> Generating a dump failed(General: %s).\n", e.what() );
							FILELOG( " -> Generating a dump failed(General: %s).", e.what() );
						}
					}

					++tmLocal.tm_mon;
					tmLocal.tm_mday = 1;
					tmLocal.tm_hour = 0;
					tmLocal.tm_min = 0;
					tmLocal.tm_sec = 0;
					tmLocal.tm_isdst = -1;

					tNextSettling = mktime( &tmLocal );
					assert( tNextSettling != -1 );

					_cprint( " -> Next settling is too close. Auto-adjusted to %I64d = %04d-%02d-%02d %02d:%02d:%02d\n", tNextSettling,
						tmLocal.tm_year + 1900, tmLocal.tm_mon + 1, tmLocal.tm_mday, tmLocal.tm_hour, tmLocal.tm_min, tmLocal.tm_sec );
					FILELOG( " -> Next settling is too close. Auto-adjusted to %I64d = %04d-%02d-%02d %02d:%02d:%02d", tNextSettling,
						tmLocal.tm_year + 1900, tmLocal.tm_mon + 1, tmLocal.tm_mday, tmLocal.tm_hour, tmLocal.tm_min, tmLocal.tm_sec );
				}
			}
			break;

		case RANKING_TYPE_HUNTAHOLIC_TOTAL:
			{
				// 헌터홀릭 토탈 점수 랭킹은 결산이라는 게 없음
				// 2100년 1월 1일로 설정
				struct tm tmLocal;

				tmLocal.tm_year = 200;
				tmLocal.tm_mon = 1;
				tmLocal.tm_mday = 1;
				tmLocal.tm_hour = 0;
				tmLocal.tm_min = 0;
				tmLocal.tm_sec = 0;
				tmLocal.tm_isdst = -1;

				tNextSettling = mktime( &tmLocal );
				assert( tNextSettling != -1 );
				pRankingInfo->m_tNextSettling = tNextSettling;
			}
			break;

		default:
			assert( 0 );
		}

		LOG::Log11N4S( LM_RANKING_SETTLE, 0, 0, eType, tCurrent, pRankingInfo->m_tNextSettling, tNextSettling, 0, 0, 0, 0, 0, "", LOG::STR_NTS, "", LOG::STR_NTS, "", LOG::STR_NTS, "", LOG::STR_NTS );

		pRankingInfo->m_tNextSettling = tNextSettling;

		switch( eType )
		{
		case RANKING_TYPE_DONATION:
			{
				// 지난 달 보상 대상자 데이터 제거
				_RANKING_INFO * pRewardRankingInfo = getRankingInfo( RANKING_TYPE_DONATION_REWARD );
				if( pRewardRankingInfo )
				{
					pRewardRankingInfo->ClearRankingData();
					Push( new DB_SettleRanking( RANKING_TYPE_DONATION_REWARD, pRankingInfo->m_tNextSettling ) );
				}
				else
					assert( 0 );

				// 최고 기록 보유자 로그 생성
				for( std::vector< _RANKING_DATA * >::const_iterator it = pRankingInfo->m_vRanking.begin() ; it != pRankingInfo->m_vRanking.end() ; ++it )
				{
					if( (*it)->m_nRank > RANKING_TOP_RECORD_MIN_RANK[ RANKING_TYPE_DONATION ] || (*it)->m_fScore < RANKING_TOP_RECORD_MIN_SCORE[ RANKING_TYPE_DONATION ] )
						break;

					// 이번 달 보상 대상자 추가
					pRewardRankingInfo->SetRankingScore( (*it)->m_nPlayerUID, StructPlayer::GetPlayerName( (*it)->m_nPlayerUID ), (*it)->m_fScore, true );
					Push( new DB_SetRankingScore( RANKING_TYPE_DONATION_REWARD, (*it)->m_nPlayerUID, (*it)->m_fScore ) );

					LOG::Log11N4S( LM_RANKING_TOP_RECORD, 0, (*it)->m_nPlayerUID, RANKING_TYPE_DONATION, (*it)->m_nRank, (*it)->m_fScore.get(), 0, 0, 0, 0, 0, 0,
						"", LOG::STR_NTS, StructPlayer::GetPlayerName( (*it)->m_nPlayerUID ), LOG::STR_NTS, "", LOG::STR_NTS, "", LOG::STR_NTS );
				}

				// 결산 처리(메모리 데이터 소거, DB 처리 요청)
				pRankingInfo->ClearRankingData();
				Push( new DB_SettleRanking( RANKING_TYPE_DONATION, pRankingInfo->m_tNextSettling ) );
			}
			break;
			
		case RANKING_TYPE_DONATION_REWARD:
			// 스코어별 랭킹 데이터가 아니므로 아무것도 하지 않음. 실제 데이터 갱신은 기부 랭킹 결산 처리 중에 모두 이루어 짐.
			break;

		case RANKING_TYPE_HUNTAHOLIC_THIS_MONTH:
			{
				// 이번 달 데이터가 지난 달 데이터가 되어야 하므로 우선 지난 달 데이터 모두 제거
				_RANKING_INFO * pLastMonthRankingInfo = getRankingInfo( RANKING_TYPE_HUNTAHOLIC_LAST_MONTH );
				if( pLastMonthRankingInfo )
				{
					pLastMonthRankingInfo->ClearRankingData();
					Push( new DB_SettleRanking( RANKING_TYPE_HUNTAHOLIC_LAST_MONTH, pRankingInfo->m_tNextSettling ) );
				}
				else
					assert( 0 );

				// 이번 달 데이터를 지난 달 데이터로 모두 이동(_RANKING_DATA들의 포인터만 옮기고 m_hsRanking만 리빌드해주면 됨. KHash가 복사 연산자가 없어서 리빌드해야됨...;;)
				// DB에서 데이터 이동(이번달 -> 지난달)은 smp_settle_ranking에서 ranking_id가 RANKING_TYPE_HUNTAHOLIC_THIS_MONTH 이면 처리함
				// 위와같이 하면 프로시저가 드러워(?)지지만 다 DELETE 하고 새로 INSERT 하는 것보다는 성능적인 이득이 매우 큼
				pLastMonthRankingInfo->m_vRanking.swap( pRankingInfo->m_vRanking );
				pRankingInfo->m_hsRanking.clear();
				for( std::vector< _RANKING_DATA * >::const_iterator it = pLastMonthRankingInfo->m_vRanking.begin() ; it != pLastMonthRankingInfo->m_vRanking.end() ; ++it )
				{
					pLastMonthRankingInfo->m_hsRanking.add( (*it)->m_nPlayerUID, (*it) );
				}

				Push( new DB_SettleRanking( RANKING_TYPE_HUNTAHOLIC_THIS_MONTH, pRankingInfo->m_tNextSettling ) );
			}
			break;

		case RANKING_TYPE_HUNTAHOLIC_LAST_MONTH:
			// 지난 달 데이터이므로 별도로 처리는 없음. 실제 데이터 갱신은 헌터홀릭 이번 달 랭킹 처리 중에 모두 이루어 짐.
			break;

		case RANKING_TYPE_HUNTAHOLIC_TOTAL:
			// 누적 데이터이므로 결산이 없음.
			break;

		default:
			assert( 0 );
		}
	}
}
