// HackShield 64비트 모듈이 없으므로 64비트 프로젝트에서는 핵실드는 지원하지 않음
#ifndef _WIN64

#include "XEnv.h"
#include "IConnection.h"
#include "StructPlayer.h"
#include "SendMessage.h"
#include "XHackShield.h"
#include "XStringUtil.h"
#include "XConsole.h"
#include "FileLog.h"

#pragma comment( lib, "AntiCpXSvr.lib" )


const DWORD XHackShield::Init()
{
	m_strErrorDescription.clear();

	if( IsInitialized() )
	{
		m_strErrorDescription = "Already initialized";
		return SS_ERROR_ALREADY_INITIALIZED;
	}

	const char * pszHashPath = ENV().GetString( "game.security_solution_init_parameter", ".\\HackShield.crc" ).c_str();

	m_hServer = _AhnHS_CreateServerObject( pszHashPath );

	if( m_hServer != ANTICPX_INVALID_HANDLE_VALUE )
	{
		m_strErrorDescription = "Error on _AhnHS_CreateServerObject";
		return SS_ERROR_INIT_FAILED;
	}

	m_bInit = true;

	return SS_ERROR_SUCCESS;
}

const DWORD XHackShield::DeInit()
{
	m_strErrorDescription.clear();

	if( !IsInitialized() )
	{
		m_strErrorDescription = "Not initialized";
		return SS_ERROR_NOT_INITIALIZED;
	}

	if( m_hServer == ANTICPX_INVALID_HANDLE_VALUE )
	{
		assert( 0 );
		m_bInit = false;
		m_strErrorDescription = "Not initialized(handle)";
		return SS_ERROR_NOT_INITIALIZED;
	}

	m_bInit = false;
	_AhnHS_CloseServerHandle( m_hServer );
	m_hServer = ANTICPX_INVALID_HANDLE_VALUE;

	return SS_ERROR_SUCCESS;
}

const DWORD XHackShield::InitClientSession( struct IStreamSocketConnection * pConnection )
{
	m_strErrorDescription.clear();

	if( !IsInitialized() || m_hServer == ANTICPX_INVALID_HANDLE_VALUE )
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

	AHNHS_CLIENT_HANDLE hClientSession = _AhnHS_CreateClientObject( m_hServer );
	if( hClientSession == ANTICPX_INVALID_HANDLE_VALUE )
	{
		m_strErrorDescription = "Error on _AhnHS_CreateClientObject";
		return SS_ERROR_INIT_FAILED;
	}

	ClientSessionInfo * pSession = new ClientSessionInfo;
	pSession->m_hClientSession = hClientSession;
	pTag->pSecuritySolutionBuffer = pSession;

	return SS_ERROR_SUCCESS;
}

const bool XHackShield::IsInitCompletedClientSession( struct IStreamSocketConnection * pConnection ) const
{
	m_strErrorDescription.clear();

	if( !IsInitialized() || m_hServer == ANTICPX_INVALID_HANDLE_VALUE )
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
	if( !pSession || pSession->m_hClientSession == ANTICPX_INVALID_HANDLE_VALUE )
	{
		m_strErrorDescription = "CS not initialized";
		return false;
	}

	if( !pSession->m_bInitComplete )
	{
		m_strErrorDescription = "Initialization not complete";
		return false;
	}

	return true;
}

const DWORD XHackShield::DeinitClientSession( struct IStreamSocketConnection * pConnection )
{
	m_strErrorDescription.clear();

	if( !IsInitialized() || m_hServer == ANTICPX_INVALID_HANDLE_VALUE )
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
	if( !pSession || pSession->m_hClientSession == ANTICPX_INVALID_HANDLE_VALUE )
	{
		m_strErrorDescription = "CS not initialized";
		return SS_ERROR_NOT_INITIALIZED;
	}

	pTag->pSecuritySolutionBuffer = NULL;

	_AhnHS_CloseClientHandle( pSession->m_hClientSession );
	delete pSession;

	return SS_ERROR_SUCCESS;
}

const DWORD XHackShield::ProcValidation( struct IStreamSocketConnection * pConnection )
{
	m_strErrorDescription.clear();

	if( !IsInitialized() || m_hServer == ANTICPX_INVALID_HANDLE_VALUE )
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
	if( !pSession || pSession->m_hClientSession == ANTICPX_INVALID_HANDLE_VALUE )
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
			TS_SC_ANTI_HACK msg;

			if( pSession->m_bNeedNewRequestMsg )
			{
				DWORD nError = _AhnHS_MakeRequest( pSession->m_hClientSession, &msg.AhnHSReqBuffer );
				if( nError != ERROR_SUCCESS )
				{
					XStringUtil::Format( m_strErrorDescription, "Error on _AhnHS_MakeRequest(%lu).", nError );
					return SS_ERROR_CREATING_REQUEST_FAILED;
				}

				s_memcpy( &pSession->m_RequestBuffer, sizeof( pSession->m_RequestBuffer ), &msg.AhnHSReqBuffer, sizeof( pSession->m_RequestBuffer ) );
			}
			else
			{
				s_memcpy( &msg.AhnHSReqBuffer, sizeof( msg.AhnHSReqBuffer ), &pSession->m_RequestBuffer, sizeof( msg.AhnHSReqBuffer ) );
			}

			pSession->m_eStatus = STATUS_WAITING_RESPONSE;

			PendMessage( pConnection, &msg );
		}
		break;
	case STATUS_WAITING_RESPONSE:
		// 클라/서버 체크를 클라에 요청 후 타임아웃 체크
		if( pSession->m_nNextRequestTime + GameRule::nSecuritySolutionResponseTimeout < tCurrent )
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

		_cprint( "HackShield: Unknown status information detected(%d). [%s/%s]\n", pSession->m_eStatus, pTag->szAccountName, pConnection->GetPeerAddress().GetAddr() );
		FILELOG( "HackShield: Unknown status information detected(%d). [%s/%s]", pSession->m_eStatus, pTag->szAccountName, pConnection->GetPeerAddress().GetAddr() );

		pSession->m_eStatus = STATUS_IDLE;
		pSession->m_nNextRequestTime = 0;
		break;
	}

	return SS_ERROR_SUCCESS;
}

const DWORD XHackShield::OnValidationResponse( struct IStreamSocketConnection * pConnection, const void * pResponseBuffer, unsigned int size )
{
	m_strErrorDescription.clear();

	if( !IsInitialized() || m_hServer == ANTICPX_INVALID_HANDLE_VALUE )
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
	if( !pSession || pSession->m_hClientSession == ANTICPX_INVALID_HANDLE_VALUE )
	{
		m_strErrorDescription = "CS not initialized";
		return SS_ERROR_NOT_INITIALIZED;
	}

	pSession->m_eStatus = STATUS_IDLE;
	pSession->m_nNextRequestTime = GetArTime() + GameRule::nPeriodOfSecuritySolutionCheck;
	pSession->m_bNeedNewRequestMsg = true;

	AHNHS_TRANS_BUFFER * pBuffer = const_cast< AHNHS_TRANS_BUFFER * >( static_cast< const AHNHS_TRANS_BUFFER * >( pResponseBuffer ) );
	DWORD nError = _AhnHS_VerifyResponse( pSession->m_hClientSession, pBuffer->byBuffer, pBuffer->nLength );
	if( nError != ERROR_SUCCESS )
	{
		XStringUtil::Format( m_strErrorDescription, "Error on _AhnHS_VerifyResponse(%lu)", nError );
		return SS_ERROR_INVALID_CLIENT_RESPONSE;
	}

	// 최초로 클라이언트로부터 응답이 왔을 때에만 해당 클라이언트 세션의 초기화가 완료된 것으로 간주(prefetch로 처리될 수 있게 조건 검사를 안 하고 매 번 덮어 씀)
	pSession->m_bInitComplete = true;

	return SS_ERROR_SUCCESS;
}

#endif
