
#ifdef	USE_GAME_GUARD_AUTH_3

#include <toolkit/XEnv.h>
#include <network/IConnection.h>
#include <toolkit/XStringUtil.h>
#include <toolkit/XConsole.h>
#include <logging/FileLog.h>

#include "StructPlayer.h"
#include "SendMessage.h"
#include "XGameGuard3.h"

#ifdef _WIN64
#pragma comment( lib, "ggsrv30lib_x64_MT.lib" )
#else
#pragma comment( lib, "ggsrv30lib_x86_MT.lib" )
#endif



// LOG_DEBUG 의 경우는 최대 10240 byte 정도의 큰 로그가 남을 수 있으므로 Buffer Overflow 에
// 주의하여 구현해 주시기 바랍니다.
void __stdcall CS3LogCallback( int nLogType, char *szLog )
{
	// 로그가 발생 시 callback 함수로 로그를 전달합니다.
	// callback 함수가 등록되어 있지 않다면 호출되지 않습니다.
	// 게임가드 프로토콜 버전, 내부버전의업데이트가발생했을때호출됩니다. (호출빈도 낮음)
	if( nLogType == LOG_UPDATE )
		FileLogHandler::GetFileLogHandler()->LogStringEx( NULL, "GameGuardLog", "this is update log : %s", szLog );
	// 초기화상황, 일반적인로그, 에러발생등의로그가발생했을때호출됩니다. (호출빈도 보통)
	else if( nLogType == LOG_NORMAL )
		FileLogHandler::GetFileLogHandler()->LogStringEx( NULL, "GameGuardLog", "this is normal log : %s", szLog );
	/*
	// 모든 유저의 패킷 정보가 남게 됩니다. (호출빈도,양 매우 높음)
	else if( nLogType == LOG_DEBUG )
		FileLogHandler::GetFileLogHandler()->LogStringEx( NULL, "GameGuardLog", "this is debug log : %s\n", szLog );
	*/
}

void __stdcall CS3UpdateInfoCallback( int nUpdateType, int nBefore, int nAfter )
{
	if( nUpdateType == UPDATE_PROTOCOL )
		FileLogHandler::GetFileLogHandler()->LogStringEx( NULL, "GameGuardLog", "this is protocol update notification : %d -> %d", nBefore, nAfter);
	else if( nUpdateType == UPDATE_GGVERSION )
		FileLogHandler::GetFileLogHandler()->LogStringEx( NULL, "GameGuardLog", "this is ggversion update notification : %d -> %d", nBefore, nAfter);
}


const DWORD XGameGuard3::Init()
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
	/*
	사용안함(구버전 인증)
	UINT32			dwNumActive			= ( vTokens.size() >= 2 ) ? atoi( vTokens[ 1 ].c_str() ) : 400;
	int				nLogType			= ( vTokens.size() >= 3 ) ? atoi( vTokens[ 2 ].c_str() ) : 0;
	int				nUpdateTimeLimit	= ( vTokens.size() >= 4 ) ? atoi( vTokens[ 3 ].c_str() ) : 30;
	int				nUpdateCondition	= ( vTokens.size() >= 5 ) ? atoi( vTokens[ 4 ].c_str() ) : 50;
	*/

	UINT32 nError = InitCSAuth3( pszModulePath );
	if( nError != ERROR_SUCCESS )
	{
		XStringUtil::Format( m_strErrorDescription, "Error on InitCSAuth3(%lu)", nError );
		return SS_ERROR_INIT_FAILED;
	}

	// Log callback 함수등록
	SetCallbackFunction( CALLBACK_LOG, (PVOID)CS3LogCallback );
	// Update callback 함수등록
	SetCallbackFunction( CALLBACK_UPDATE, (PVOID)CS3UpdateInfoCallback );

	m_bInit = true;
	return SS_ERROR_SUCCESS;
}

const DWORD XGameGuard3::DeInit()
{
	m_strErrorDescription.clear();

	if( !IsInitialized() )
	{
		m_strErrorDescription = "Not initialized";
		return SS_ERROR_NOT_INITIALIZED;
	}

	m_bInit = false;

	// 서버가 종료되는 경우
	CloseCSAuth3();
	return SS_ERROR_SUCCESS;
}

const DWORD XGameGuard3::InitClientSession( struct IStreamSocketConnection * pConnection )
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
	pTag->pSecuritySolutionBuffer = pSession;

	DWORD nError = sendAuthQuery( pConnection, pSession );
	if( nError != SS_ERROR_SUCCESS )
	{
		return nError;
	}

	pSession->m_nNextRequestTime = GetArTime() + GameRule::nPeriodOfSecuritySolutionCheck;
	pSession->m_bIsFirstTimeCheck = false;
	return SS_ERROR_SUCCESS;
}

const bool XGameGuard3::IsInitCompletedClientSession( struct IStreamSocketConnection * pConnection ) const
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
	if( !pSession || !pSession->m_pCCSAuth3 )
	{
		m_strErrorDescription = "CS not initialized";
		return false;
	}

	// pSession->m_bInitComplete == false 여도 일단 통과시켜서 캐릭터 선택 및 이후 진행은 가능하게 하되
	// ProcValidation에서 m_bInitComplete == false 인 경우에는 별도의 타임아웃 제한을 걸어서 체크함

	return true;
}

const DWORD XGameGuard3::DeinitClientSession( struct IStreamSocketConnection * pConnection )
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
	if( !pSession || !pSession->m_pCCSAuth3 )
	{
		m_strErrorDescription = "CS not initialized";
		return SS_ERROR_NOT_INITIALIZED;
	}

	pTag->pSecuritySolutionBuffer = NULL;
	delete pSession;

	return SS_ERROR_SUCCESS;
}

const DWORD XGameGuard3::ProcValidation( struct IStreamSocketConnection * pConnection )
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
	if( !pSession || !pSession->m_pCCSAuth3 )
	{
		m_strErrorDescription = "CS not initialized";
		return SS_ERROR_NOT_INITIALIZED;
	}

	AR_TIME tCurrent = GetArTime();
	if( pSession->m_nNextRequestTime <= tCurrent )
	{
		DWORD nError = sendAuthQuery( pConnection, pSession );
		if( nError != SS_ERROR_SUCCESS )
		{
			return nError;
		}

		pSession->m_nNextRequestTime = GetArTime() + GameRule::nPeriodOfSecuritySolutionCheck;
	}

	return SS_ERROR_SUCCESS;
}

const DWORD XGameGuard3::OnValidationResponse( struct IStreamSocketConnection * pConnection, const void * pResponseBuffer, unsigned int size )
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
	if( !pSession || !pSession->m_pCCSAuth3 )
	{
		m_strErrorDescription = "CS not initialized";
		return SS_ERROR_NOT_INITIALIZED;
	}

	if( size > sizeof( pSession->m_pCCSAuth3->packet ) )
	{
		XStringUtil::Format( m_strErrorDescription, "Error on size too big(%lu)", size );
		return SS_ERROR_INVALID_CLIENT_RESPONSE;
	}

	s_memcpy( &pSession->m_pCCSAuth3->packet, sizeof( pSession->m_pCCSAuth3->packet ), pResponseBuffer, size );
	UINT32 nError = pSession->m_pCCSAuth3->Check( size );
	if( nError >= 3000 )
	{
		XStringUtil::Format( m_strErrorDescription, "Error on Check Auth Answer(%lu)", nError );
		return SS_ERROR_INVALID_CLIENT_RESPONSE;
	}

	// from gameguard 클라이언트에서 게임서버에 접속 시 서버에서는 첫 번째 인증을 송수신 후에
	// 즉시로 두 번째 인증을 전송한다면 게임가드를 건너 뛰는 핵 툴 방식에 대하여 즉각적인 차단이 가능합니다.
	if( pSession->m_bIsFirstTimeCheck == false )
	{
		pSession->m_bIsFirstTimeCheck = true;
		DWORD nError = sendAuthQuery( pConnection, pSession );
		if( nError != SS_ERROR_SUCCESS )
		{
			return nError;
		}

		pSession->m_nNextRequestTime = GetArTime() + GameRule::nPeriodOfSecuritySolutionCheck;
	}

	return SS_ERROR_SUCCESS;
}

const DWORD XGameGuard3::sendAuthQuery( struct IStreamSocketConnection * pConnection, struct XGameGuard3::ClientSessionInfo * pSession )
{
	const int buffer_size = sizeof( pSession->m_pCCSAuth3->packet ) + sizeof( TS_SC_GAME_GUARD_AUTH_QUERY );
	char auth_msg_buffer[buffer_size] = { 0, };

	TS_SC_GAME_GUARD_AUTH_QUERY* msg = new (auth_msg_buffer) TS_SC_GAME_GUARD_AUTH_QUERY;

	UINT32 uReturnedPacketSize = 0;
	UINT32 nError = pSession->m_pCCSAuth3->Get( &uReturnedPacketSize );
	if( nError >= 3000 )
	{
		XStringUtil::Format( m_strErrorDescription, "Error on GetAuthQuery(%lu)", nError );
		return SS_ERROR_CREATING_REQUEST_FAILED;
	}

	if( uReturnedPacketSize > 0 )
	{
		msg->size += uReturnedPacketSize;
		msg->auth_data_size = MAKELONG( uReturnedPacketSize, 1 );
		s_memcpy( (msg+1), buffer_size-sizeof( *msg ), &pSession->m_pCCSAuth3->packet, uReturnedPacketSize );
		PendMessage( pConnection, msg );
	}

	return SS_ERROR_SUCCESS;
}

#endif