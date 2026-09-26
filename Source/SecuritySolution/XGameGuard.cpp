
#ifndef	USE_GAME_GUARD_AUTH_3

#include <toolkit/XEnv.h>
#include <network/IConnection.h>
#include <toolkit/XStringUtil.h>
#include <toolkit/XConsole.h>
#include <logging/FileLog.h>

#include "StructPlayer.h"
#include "SendMessage.h"
#include "XGameGuard.h"

#ifdef _WIN64
#pragma comment( lib, "ggsrvlib25_x64.lib" )
#else
#pragma comment( lib, "ggsrvlib25.lib" )
#endif


GGAUTHS_API void NpLog( int mode, char *msg )
{
	if( mode & (NPLOG_DEBUG | NPLOG_ERROR) )
		FileLogHandler::GetFileLogHandler()->LogStringEx( NULL, "GameGuardLog", "NpLog: %s", msg );
}

GGAUTHS_API void GGAuthUpdateCallback( PGG_UPREPORT report )
{
	FileLogHandler::GetFileLogHandler()->LogStringEx( NULL, "GameGuardLog", "GGAuth Version Update Report: [Type:%s][Version:%d->%d]",
		( report->nType == 1 ) ? "GameGuard Ver" : "Protocol Num", report->dwBefore, report->dwNext );
}


const DWORD XGameGuard::Init()
{
	m_strErrorDescription.clear();

	if( IsInitialized() )
	{
		m_strErrorDescription = "Already initialized";
		return SS_ERROR_ALREADY_INITIALIZED;
	}

	std::vector< std::string > vTokens;
	XStringUtil::Split( ENV().GetString( "game.security_solution_init_parameter", "" ).c_str(), vTokens, "|", false );

	char *			pszModulePath		= const_cast< char * >( ( vTokens.size() >= 1 ) ? vTokens[ 0 ].c_str() : "GameGuard" );
	UINT32			dwNumActive			= ( vTokens.size() >= 2 ) ? atoi( vTokens[ 1 ].c_str() ) : 400;
	int				nLogType			= ( vTokens.size() >= 3 ) ? atoi( vTokens[ 2 ].c_str() ) : 0;
	int				nUpdateTimeLimit	= ( vTokens.size() >= 4 ) ? atoi( vTokens[ 3 ].c_str() ) : 30;
	int				nUpdateCondition	= ( vTokens.size() >= 5 ) ? atoi( vTokens[ 4 ].c_str() ) : 50;

	UINT32 nError = InitGameguardAuth( pszModulePath, dwNumActive, false, nLogType );

	if( nError != ERROR_SUCCESS )
	{
		XStringUtil::Format( m_strErrorDescription, "Error on InitGameguardAuth(%lu)", nError );
		return SS_ERROR_INIT_FAILED;
	}

	SetUpdateCondition( nUpdateTimeLimit, nUpdateCondition );
	m_bInit = true;

	return SS_ERROR_SUCCESS;
}

const DWORD XGameGuard::DeInit()
{
	m_strErrorDescription.clear();

	if( !IsInitialized() )
	{
		m_strErrorDescription = "Not initialized";
		return SS_ERROR_NOT_INITIALIZED;
	}

	m_bInit = false;
	CleanupGameguardAuth();

	return SS_ERROR_SUCCESS;
}

const DWORD XGameGuard::InitClientSession( struct IStreamSocketConnection * pConnection )
{
	m_strErrorDescription.clear();

	if( !IsInitialized() )
	{
		m_strErrorDescription = "SS not initialized";
		return SS_ERROR_NOT_INITIALIZED;
	}

	_CONNECTION_TAG * pTag = static_cast< _CONNECTION_TAG * >( pConnection->GetTag() );
	if( !pTag )
	{
		m_strErrorDescription = "No connection tag";
		return SS_ERROR_NO_CONNECTION_TAG;
	}

	if( pTag->pSecuritySolutionBuffer )
	{
		m_strErrorDescription = "Already initialized";
		return SS_ERROR_ALREADY_INITIALIZED;
	}

	ClientSessionInfo * pSession = new ClientSessionInfo;

	// 클라이언트 세션 세팅 완료 직후 서버/클라 체크를 1회 진행함
	pSession->m_eStatus = STATUS_WAITING_RESPONSE;
	pSession->m_nNextRequestTime = GetArTime();

	pTag->pSecuritySolutionBuffer = pSession;

	DWORD nError = sendAuthQuery( pConnection, pSession );
	if( nError != SS_ERROR_SUCCESS )
	{
		pSession->m_eStatus = STATUS_IDLE;
		return nError;
	}

	return SS_ERROR_SUCCESS;
}

const bool XGameGuard::IsInitCompletedClientSession( struct IStreamSocketConnection * pConnection ) const
{
	m_strErrorDescription.clear();

	if( !IsInitialized() )
	{
		m_strErrorDescription = "SS not initialized";
		return false;
	}

	_CONNECTION_TAG * pTag = static_cast< _CONNECTION_TAG * >( pConnection->GetTag() );
	if( !pTag )
	{
		m_strErrorDescription = "No connection tag";
		return false;
	}

	ClientSessionInfo * pSession = static_cast< ClientSessionInfo * >( pTag->pSecuritySolutionBuffer );
	if( !pSession || !pSession->m_pCCSAuth2 )
	{
		m_strErrorDescription = "CS not initialized";
		return false;
	}

	// pSession->m_bInitComplete == false 여도 일단 통과시켜서 캐릭터 선택 및 이후 진행은 가능하게 하되
	// ProcValidation에서 m_bInitComplete == false 인 경우에는 별도의 타임아웃 제한을 걸어서 체크함

	return true;
}

const DWORD XGameGuard::DeinitClientSession( struct IStreamSocketConnection * pConnection )
{
	m_strErrorDescription.clear();

	if( !IsInitialized() )
	{
		m_strErrorDescription = "SS not initialized";
		return SS_ERROR_NOT_INITIALIZED;
	}

	_CONNECTION_TAG * pTag = static_cast< _CONNECTION_TAG * >( pConnection->GetTag() );
	if( !pTag )
	{
		m_strErrorDescription = "No connection tag";
		return SS_ERROR_NO_CONNECTION_TAG;
	}

	ClientSessionInfo * pSession = static_cast< ClientSessionInfo * >( pTag->pSecuritySolutionBuffer );
	if( !pSession || !pSession->m_pCCSAuth2 )
	{
		m_strErrorDescription = "CS not initialized";
		return SS_ERROR_NOT_INITIALIZED;
	}

	pTag->pSecuritySolutionBuffer = NULL;
	delete pSession;

	return SS_ERROR_SUCCESS;
}

const DWORD XGameGuard::ProcValidation( struct IStreamSocketConnection * pConnection )
{
	m_strErrorDescription.clear();

	if( !IsInitialized() )
	{
		m_strErrorDescription = "SS not initialized";
		return SS_ERROR_NOT_INITIALIZED;
	}

	_CONNECTION_TAG * pTag = static_cast< _CONNECTION_TAG * >( pConnection->GetTag() );
	if( !pTag )
	{
		m_strErrorDescription = "No connection tag";
		return SS_ERROR_NO_CONNECTION_TAG;
	}

	ClientSessionInfo * pSession = static_cast< ClientSessionInfo * >( pTag->pSecuritySolutionBuffer );
	if( !pSession || !pSession->m_pCCSAuth2 )
	{
		m_strErrorDescription = "CS not initialized";
		return SS_ERROR_NOT_INITIALIZED;
	}

	AR_TIME tCurrent = GetArTime();

	switch( pSession->m_eStatus )
	{
	case STATUS_IDLE:
		// 주기적으로 클라/서버 체크 진행
		if( pSession->m_nNextRequestTime <= tCurrent )
		{
			pSession->m_eStatus = STATUS_WAITING_RESPONSE;
			DWORD nError = sendAuthQuery( pConnection, pSession );
			if( nError != SS_ERROR_SUCCESS )
			{
				pSession->m_eStatus = STATUS_IDLE;
				return nError;
			}
		}
		break;
	case STATUS_WAITING_RESPONSE:
		// 클라/서버 체크를 클라에 요청 후 타임아웃 체크

		// 최초 인증에 대한 체크는 별도의 타임아웃 및 제한을 적용
		if( !pSession->m_bInitComplete )
		{
			if( pSession->m_nNextRequestTime + INITIAL_VALIDATION_TIMEOUT < tCurrent )
			{
				pSession->m_eStatus = STATUS_IDLE;
				pSession->m_nNextRequestTime = tCurrent + GameRule::nPeriodOfSecuritySolutionCheck;
				pSession->m_bNeedNewRequestMsg = false;

				XStringUtil::Format( m_strErrorDescription, "Initial validation timed out" );
				return SS_ERROR_CLIENT_RESPONSE_TIMEOUT;
			}
		}
		// 일반 인증의 경우 지정된 횟수만큼 타임아웃을 허용하는 식으로 처리
		else if( pSession->m_nNextRequestTime + GameRule::nSecuritySolutionResponseTimeout < tCurrent )
		{
			pSession->m_eStatus = STATUS_IDLE;
			pSession->m_nNextRequestTime = tCurrent + GameRule::nPeriodOfSecuritySolutionCheck;
			pSession->m_bNeedNewRequestMsg = false;

			int nTimeCount = 1;
			if( !m_hsResponseTimeoutCount.lookup( pTag->nAccountID, nTimeCount ) )
				m_hsResponseTimeoutCount.add( pTag->nAccountID, nTimeCount );
			else
				m_hsResponseTimeoutCount.modify( pTag->nAccountID, ++nTimeCount );

			if( nTimeCount >= MAX_CLIENT_RESPONSE_TIMEOUT_COUNT )
			{
				XStringUtil::Format( m_strErrorDescription, "Timed out %d times", nTimeCount );
				return SS_ERROR_CLIENT_RESPONSE_TIMEOUT;
			}
		}
		break;
	default:
		// 뭔 상태인지 알 수 없는 경우에는(있어서는 안되지만) 다음 Proc 돌 때 무조건 체크 진행하도록 세팅
		assert( 0 );

		_cprint( "GameGuard: Unknown status information detected(%d). [%s/%s]\n", pSession->m_eStatus, pTag->szAccountName, pConnection->GetPeerAddress().GetAddr() );
		FILELOG( "GameGuard: Unknown status information detected(%d). [%s/%s]", pSession->m_eStatus, pTag->szAccountName, pConnection->GetPeerAddress().GetAddr() );

		pSession->m_eStatus = STATUS_IDLE;
		pSession->m_nNextRequestTime = 0;
		break;
	}

	return SS_ERROR_SUCCESS;
}

const DWORD XGameGuard::OnValidationResponse( struct IStreamSocketConnection * pConnection, const void * pResponseBuffer, unsigned int size )
{
	m_strErrorDescription.clear();

	if( !IsInitialized() )
	{
		m_strErrorDescription = "SS not initialized";
		return SS_ERROR_NOT_INITIALIZED;
	}

	_CONNECTION_TAG * pTag = static_cast< _CONNECTION_TAG * >( pConnection->GetTag() );
	if( !pTag )
	{
		m_strErrorDescription = "No connection tag";
		return SS_ERROR_NO_CONNECTION_TAG;
	}

	ClientSessionInfo * pSession = static_cast< ClientSessionInfo * >( pTag->pSecuritySolutionBuffer );
	if( !pSession || !pSession->m_pCCSAuth2 )
	{
		m_strErrorDescription = "CS not initialized";
		return SS_ERROR_NOT_INITIALIZED;
	}

	pSession->m_eStatus = STATUS_IDLE;
	pSession->m_nNextRequestTime += GameRule::nPeriodOfSecuritySolutionCheck;
	pSession->m_bNeedNewRequestMsg = true;

	s_memcpy( &pSession->m_pCCSAuth2->m_AuthAnswer, sizeof( pSession->m_pCCSAuth2->m_AuthAnswer ), pResponseBuffer, sizeof( pSession->m_pCCSAuth2->m_AuthAnswer ) );
	UINT32 nError = pSession->m_pCCSAuth2->CheckAuthAnswer();
	if( nError != ERROR_SUCCESS )
	{
		XStringUtil::Format( m_strErrorDescription, "Error on CheckAuthAnswer(%lu)", nError );
		return SS_ERROR_INVALID_CLIENT_RESPONSE;
	}

	// 첫번째 요청은 버전 인증으로 최초 인증시에는 2번 연속으로 인증을 해야 제대로 된 인증이 이루어 짐
	if( !pSession->m_bInitComplete )
	{
		pSession->m_eStatus = STATUS_WAITING_RESPONSE;
		pSession->m_bInitComplete = true;
		pSession->m_nNextRequestTime = GetArTime();

		DWORD nError = sendAuthQuery( pConnection, pSession );
		if( nError != SS_ERROR_SUCCESS )
		{
			pSession->m_eStatus = STATUS_IDLE;
			return nError;
		}
	}

	return SS_ERROR_SUCCESS;
}

const DWORD XGameGuard::sendAuthQuery( struct IStreamSocketConnection * pConnection, struct XGameGuard::ClientSessionInfo * pSession )
{
	const int buffer_size = sizeof( GG_AUTH_DATA ) + sizeof( TS_SC_GAME_GUARD_AUTH_QUERY );
	char auth_msg_buffer[buffer_size] = { 0, };

	TS_SC_GAME_GUARD_AUTH_QUERY* msg = new (auth_msg_buffer) TS_SC_GAME_GUARD_AUTH_QUERY;

	if( pSession->m_bNeedNewRequestMsg )
	{
		UINT32 nError = pSession->m_pCCSAuth2->GetAuthQuery();
		if( nError != ERROR_SUCCESS )
		{
			XStringUtil::Format( m_strErrorDescription, "Error on GetAuthQuery(%lu)", nError );
			return SS_ERROR_CREATING_REQUEST_FAILED;
		}
	}

	s_memcpy( (msg+1), buffer_size - sizeof( *msg ), &pSession->m_pCCSAuth2->m_AuthQuery, sizeof( GG_AUTH_DATA ) );
	msg->auth_data_size = sizeof( GG_AUTH_DATA );
	msg->size += msg->auth_data_size;

	PendMessage( pConnection, msg );

	return SS_ERROR_SUCCESS;
}

#endif
