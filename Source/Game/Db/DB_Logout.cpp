#include "DB_Commands.h"

#include "StructPlayer.h"
#include "GameRule.h"

bool DB_Logout::onProcess( DBConnection &db )
{
	try
	{
		{
			_CommandPtr cmd;
			if( db.CreateCommand( cmd ) == false )	throw XException( "DB_Logout : CreateInstance(command) error" );

			cmd->CommandType = adCmdStoredProc;
			cmd->CommandText = _bstr_t( "dbo.smp_logout_character" );
			// Store the name of current stored-procedure for debugging
			szStoredProcedureName = "dbo.smp_logout_character";

			cmd->Parameters->Append( cmd->CreateParameter( "IN_SID", adInteger, adParamInput, 4, m_pPlayer->GetPlayerUID() ) );
			cmd->Parameters->Append( cmd->CreateParameter( "IN_PLAYTIME", adInteger, adParamInput, 4, m_pPlayer->GetLoginDuration() ) );

			cmd->Execute( NULL, NULL, adCmdStoredProc);
		}

		// 플레이 포인트 관련
		if( GameRule::bUsePlayPoint )
		{
			_CommandPtr cmd;
			if( db.CreateCommand( cmd ) == false )	throw XException( "DB_Logout : CreateInstance(command) error" );

			cmd->CommandType = adCmdStoredProc;
			cmd->CommandText = _bstr_t( "dbo.smp_update_play_time_point" );
			// Store the name of current stored-procedure for debugging
			szStoredProcedureName = "dbo.smp_update_play_time_point";

			AR_TIME tLastPlayTimeCalculateTime = m_pPlayer->GetLogoutTime();
			if( !tLastPlayTimeCalculateTime )
			{
				tLastPlayTimeCalculateTime = GetArTime();
			}
			int nPlayTimeInMin = ( tLastPlayTimeCalculateTime - m_pPlayer->GetLastPlayTimeUpdateTime() ) / ( 100 * 60 );

			// 서버에서 포인트 누적 단위 시간 마다 저장하기 때문에 일반적으로는 여기서는 포인트가 쌓일 일은 없음
			// 단, OnUpdate의 호출 타이밍이 늦은 경우(ArSchedulerObject의 Priority가 낮을 때)
			// 갱신 시각을 이미 지났으나 OnUpdate가 아직 호출되지 않은 시점에 로그아웃하면 쌓일 수 있음
			int nPoint = ( nPlayTimeInMin / GameRule::nPlayPointAccumulateTerm ) * GameRule::nPlayPointAccumulateAmount;
			if( m_pPlayer->GetPCBangMode() == GameRule::PCBANG_PREMIUM_BONUS )
			{
				nPoint *= GameRule::fPremiumPCBangPlayPointBonusRate;
			}

			cmd->Parameters->Append( cmd->CreateParameter( "IN_ACCOUNT_ID", adInteger, adParamInput, 4, m_pPlayer->GetAccountID() ) );
			cmd->Parameters->Append( cmd->CreateParameter( "IN_PLAY_TIME", adInteger, adParamInput, 4, nPlayTimeInMin ) );
			cmd->Parameters->Append( cmd->CreateParameter( "IN_POINT", adInteger, adParamInput, 4, nPoint ) );

			cmd->Execute( NULL, NULL, adCmdStoredProc );
		}

		m_pPlayer->onEndQuery();
	}
	catch( ... )
	{
		m_pPlayer->onEndQuery();
		throw;
	}

	return true;
}
