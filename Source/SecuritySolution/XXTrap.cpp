
#include <toolkit/XEnv.h>
#include <network/IConnection.h>
#include <toolkit/XStringUtil.h>
#include <toolkit/XConsole.h>
#include <logging/FileLog.h>

#include "StructPlayer.h"
#include "SendMessage.h"
#include "XXTrap.h"


// xtrap_server
#ifdef _WIN64
	#ifdef _DEBUG
	#pragma comment( lib, "xtrap_serverd_x64.lib" )
	#else
	#pragma comment( lib, "xtrap_server_x64.lib" )
	#endif
#else
	#ifdef _DEBUG
	#pragma comment( lib, "xtrap_serverd.lib" )
	#else
	#pragma comment( lib, "xtrap_server.lib" )
	#endif
#endif


const DWORD XXTrap::Init()
{
	m_strErrorDescription.clear();

	if( IsInitialized() )
	{
		m_strErrorDescription = "Already initialized";
		return SS_ERROR_ALREADY_INITIALIZED;
	}

	std::vector< std::string > vTokens;
	XStringUtil::Split( ENV().GetString( "game.security_solution_init_parameter", "2|map1.cs3|map2.cs3" ).c_str(), vTokens, "|", false );

	if( vTokens.size() <= 1 )
	{
		m_strErrorDescription = "Invalid initialization parameter";
		return SS_ERROR_INIT_FAILED;
	}

	m_nMapCount = atoi( vTokens[ 0 ].c_str() );
	if( vTokens.size() < m_nMapCount + 1 )
		m_nMapCount = vTokens.size() - 1;

	if( !m_nMapCount )
	{
		m_strErrorDescription = "Invalid initialization parameter";
		return SS_ERROR_INIT_FAILED;
	}

	if( !m_pszXTrapMap )
	{
		m_pszXTrapMap = new unsigned char [ m_nMapCount * XTRAP_CS4_BUFSIZE_MAP ];
	}

	for( size_t nFileIndex = 0 ; nFileIndex < m_nMapCount ; ++nFileIndex )
	{
		FILE * fp = NULL;
		if( fopen_s( &fp, vTokens[ nFileIndex + 1 ].c_str(), "rb" ) )
		{
			m_strErrorDescription = "File open failed: ";
			m_strErrorDescription += vTokens[ nFileIndex + 1 ];

			delete [] m_pszXTrapMap;
			m_pszXTrapMap = NULL;
			return SS_ERROR_INIT_FAILED;
		}

		if( !fread_s( m_pszXTrapMap + ( nFileIndex * XTRAP_CS4_BUFSIZE_MAP ), XTRAP_CS4_BUFSIZE_MAP, XTRAP_CS4_BUFSIZE_MAP, 1, fp ) )
		{
			m_strErrorDescription = "File read failed: ";
			m_strErrorDescription += vTokens[ nFileIndex + 1 ];

			delete [] m_pszXTrapMap;
			m_pszXTrapMap = NULL;
			return SS_ERROR_INIT_FAILED;
		}

		fclose( fp );
	}

	unsigned int nError = XTrap_S_LoadDll();
	if( nError )
	{
		XStringUtil::Format( m_strErrorDescription, "XTrap_S_LoadDll failed: 0x%08X", nError );
		delete [] m_pszXTrapMap;
		m_pszXTrapMap = NULL;
		return SS_ERROR_INIT_FAILED;
	}

	nError = XTrap_S_Start( 600, static_cast< unsigned int >( m_nMapCount ), m_pszXTrapMap, NULL );
	if( nError )
	{
		XStringUtil::Format( m_strErrorDescription, "Error on XTrap_S_Start(0x%08X)", nError );
		delete [] m_pszXTrapMap;
		m_pszXTrapMap = NULL;
		return SS_ERROR_INIT_FAILED;
	}

	// 클라/서버 인증 처리를 20초에 한 번씩 발생시킴(XTrap 요구 사항)
	if( GameRule::nPeriodOfSecuritySolutionCheck > 2000 )
	{
		assert( 0 );
		GameRule::nPeriodOfSecuritySolutionCheck = 2000;
	}

	m_bInit = true;

	return SS_ERROR_SUCCESS;
}

const DWORD XXTrap::DeInit()
{
	m_strErrorDescription.clear();

	if( !IsInitialized() )
	{
		m_strErrorDescription = "Not initialized";
		return SS_ERROR_NOT_INITIALIZED;
	}

	if( !m_pszXTrapMap )
	{
		assert( 0 );
		m_bInit = false;
		m_strErrorDescription = "Not initialized(Map buffer)";
		return SS_ERROR_NOT_INITIALIZED;
	}

	m_bInit = false;

	XTrap_S_FreeDll();
	delete [] m_pszXTrapMap;
	m_pszXTrapMap = NULL;

	return SS_ERROR_SUCCESS;
}

const DWORD XXTrap::InitClientSession( struct IStreamSocketConnection * pConnection )
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
	unsigned int nError = XTrap_S_SessionInit( 600, static_cast< unsigned int >( m_nMapCount ), m_pszXTrapMap, pSession->m_szClientSession );
	if( nError )
	{
		XStringUtil::Format( m_strErrorDescription, "Error on XTrap_S_SessionInit(0x%08X)", nError );
		delete pSession;
		return SS_ERROR_INIT_FAILED;
	}

	pTag->pSecuritySolutionBuffer = pSession;

	// 별도의 최초 인증 과정 없이 로그인을 허용하고 주기적인 검사를 진행하도록 초기화 완료 체크
	pSession->m_bInitComplete = true;

	return SS_ERROR_SUCCESS;
}

const bool XXTrap::IsInitCompletedClientSession( struct IStreamSocketConnection * pConnection ) const
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
	if( !pSession )
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

const DWORD XXTrap::DeinitClientSession( struct IStreamSocketConnection * pConnection )
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
	if( !pSession )
	{
		m_strErrorDescription = "CS not initialized";
		return SS_ERROR_NOT_INITIALIZED;
	}

	pTag->pSecuritySolutionBuffer = NULL;
	delete pSession;

	return SS_ERROR_SUCCESS;
}

const DWORD XXTrap::ProcValidation( struct IStreamSocketConnection * pConnection )
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
	if( !pSession )
	{
		m_strErrorDescription = "CS not initialized";
		return SS_ERROR_NOT_INITIALIZED;
	}

	AR_TIME tCurrent = GetArTime();

	switch( pSession->m_eStatus )
	{
	case STATUS_IDLE:
	case STATUS_WAITING_RESPONSE:
		// 주기적으로 클라/서버 체크 진행, 클라에서 응답을 줬든 말든 무조건 주기적으로 요청 메시지를 보냄(XTrap 요구 사항)
		// XTrap은 자체적으로 Timeout 체크 기능을 사용 하므로 엔진에서 별도 처리를 하면 안 됨(XTrap 요구 사항)
		if( pSession->m_nNextRequestTime <= tCurrent )
		{
			TS_SC_XTRAP_CHECK msg;

			unsigned int nError = XTrap_CS_Step1( pSession->m_szClientSession, msg.pCheckBuffer );

			PendMessage( pConnection, &msg );

			// XTrap_CS_STEP1 함수에서 오류 코드가 반환되어도 패킷을 클라에게 송신한 이후에 오류 처리를 해야 함(XTrap 요구 사항)
			if( !nError )
			{
				pSession->m_nNextRequestTime = tCurrent + GameRule::nPeriodOfSecuritySolutionCheck;
			}
			else
			{
				XStringUtil::Format( m_strErrorDescription, "Error on XTrap_CS_Step1(%u:%s)", nError, getErrorDescription( nError ) );
				return SS_ERROR_CREATING_REQUEST_FAILED;
			}
		}
		break;
	default:
		// 뭔 상태인지 알 수 없는 경우에는(있어서는 안되지만) 다음 Proc 돌 때 무조건 체크 진행하도록 세팅
		assert( 0 );

		_cprint( "XTrap: Unknown status information detected(%d). [%s/%s]\n", pSession->m_eStatus, pTag->szAccountName, pConnection->GetPeerAddress().GetAddr() );
		FILELOG( "XTrap: Unknown status information detected(%d). [%s/%s]", pSession->m_eStatus, pTag->szAccountName, pConnection->GetPeerAddress().GetAddr() );

		pSession->m_eStatus = STATUS_IDLE;
		pSession->m_nNextRequestTime = 0;
		break;
	}

	return SS_ERROR_SUCCESS;
}

const DWORD XXTrap::OnValidationResponse( struct IStreamSocketConnection * pConnection, const void * pResponseBuffer, unsigned int size )
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
	if( !pSession )
	{
		m_strErrorDescription = "CS not initialized";
		return SS_ERROR_NOT_INITIALIZED;
	}

	unsigned int nError = XTrap_CS_Step3( pSession->m_szClientSession, pResponseBuffer );
	if( nError != ERROR_SUCCESS )
	{
		XStringUtil::Format( m_strErrorDescription, "Error on XTrap_CS_Step3(%u:%s)", nError, getErrorDescription( nError ) );
		// XTrap_CS_Step3 함수에서 오류가 나도 아무 처리하지 않고 다음 XTrap_CS_Step1 함수에서 처리되도록 현재 상태를 유지해야 함(XTrap 요구 사항)
	}

	// 최초로 클라이언트로부터 응답이 왔을 때에만 해당 클라이언트 세션의 초기화가 완료된 것으로 간주(prefetch로 처리될 수 있게 조건 검사를 안 하고 매 번 덮어 씀)
	pSession->m_bInitComplete = true;

	return SS_ERROR_SUCCESS;
}

const char * XXTrap::getErrorDescription( const unsigned int nError ) const
{
	const char * pszDescription = "Unknown error code";

	switch( nError )
	{
	case 1:
		pszDescription = "Error";
		break;
	case 2:
		pszDescription = "Unknown";
		break;
	case 3:
		pszDescription = "Invalid parameter";
		break;
	case 4:
		pszDescription = "Checksum error";
		break;
	case 5:
		pszDescription = "Timeout occured";
		break;
	case 15:
		pszDescription = "Hack detected";
		break;
	}

	return pszDescription;
}
