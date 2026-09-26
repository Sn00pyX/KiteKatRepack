
#include <network/IConnection.h>
#include <toolkit/XStringUtil.h>
#include <toolkit/nsl.h>
#include <dump/XException.h>
#include <toolkit/XConsole.h>
#include <logging/FileLog.h>
#include <mmo/ArcadiaServer.h>

#include "LogClient/LogClient.h"

#include "XSecuritySolutionManager.h"

// TODO 게임가드3 인증지원시 2버전꺼 지우자
//전처리기에 USE_GAME_GUARD_AUTH_3 선언해주자
#ifdef	USE_GAME_GUARD_AUTH_3
#include "XGameGuard3.h"
#else
#include "XGameGuard.h"
#endif

#ifndef _WIN64		// 64비트용 핵실드 모듈은 보유중인 게 없으므로 아예 포함시키지 않음
#include "XHackShield.h"
#endif
#include "XXTrap.h"
#include "GameRule.h"
#include "StructPlayer.h"
#include "SendMessage.h"
#include "Constant.h"


XSecuritySolutionManager::~XSecuritySolutionManager()
{
	Deinit();
}

const bool XSecuritySolutionManager::Init()
{
	if( m_pImpl )
	{
		assert( 0 );
		return false;
	}

	if( GameRule::nSecuritySolutionType == TYPE_NONE )
		return true;

	switch( GameRule::nSecuritySolutionType )
	{
	case TYPE_NONE:
		// 보안 솔루션 없음
		break;
	case TYPE_GAMEGUARD:
#ifdef	USE_GAME_GUARD_AUTH_3
		m_pImpl = new XGameGuard3();
#else
		m_pImpl = new XGameGuard();
#endif
		break;
	case TYPE_HACKSHIELD:
#ifndef _WIN64
		m_pImpl = new XHackShield();
#endif
		break;
	case TYPE_XTRAP:
		m_pImpl = new XXTrap();
		break;
	default:
		assert( 0 );
		break;
	}

	std::string strResult;
	if( !m_pImpl )
	{
		XStringUtil::Format( strResult, "Initializing a security solution failed.[SecuritySolutionType:%d/Detail:Not supported type]\n", GameRule::nSecuritySolutionType );

		_cprint( strResult.c_str() );
		strResult.erase( strResult.length() - 1 );
		FileLogHandler::GetFileLogHandler()->LogStringEx( NULL, "SecuritySolution", strResult.c_str() );

		throw XException( strResult.c_str() );
		return false;
	}

	DWORD nError = m_pImpl->Init();
	if( nError != SS_ERROR_SUCCESS )
	{
		XStringUtil::Format( strResult, "%s: Initializing failed.[ErrorCode:%d/Detail:%s]\n", m_pImpl->GetName(), nError, ( !strlen( m_pImpl->GetErrorDescription() ) ? "No detail" : m_pImpl->GetErrorDescription() ) );

		_cprint( strResult.c_str() );
		strResult.erase( strResult.length() - 1 );
		FileLogHandler::GetFileLogHandler()->LogStringEx( NULL, "SecuritySolution", strResult.c_str() );

		Deinit();

		throw XException( strResult.c_str() );
		return false;
	}

	XStringUtil::Format( strResult, "%s: Initializing complete.\n", m_pImpl->GetName() );
	_cprint( strResult.c_str() );
	strResult.erase( strResult.length() - 1 );
	FileLogHandler::GetFileLogHandler()->LogStringEx( NULL, "SecuritySolution", strResult.c_str() );

	return true;
}

const void XSecuritySolutionManager::Deinit()
{
	if( !m_pImpl )
		return;

	if( m_pImpl->IsInitialized() )
		m_pImpl->DeInit();

	delete m_pImpl;
	m_pImpl = NULL;
}

const bool XSecuritySolutionManager::InitClientSession( struct IStreamSocketConnection * pConnection )
{
	if( !m_pImpl )
		return true;

	if( isExceptionalIP( pConnection->GetPeerAddress().GetAddr() ) )
	{
		_cprint( "Security Solution exceptional ip login: %s\n", pConnection->GetPeerAddress().GetAddr() );
		FILELOG( "Security Solution exceptional ip login: %s", pConnection->GetPeerAddress().GetAddr() );
		return true;
	}

	DWORD nError = m_pImpl->InitClientSession( pConnection );
	if( nError != SS_ERROR_SUCCESS )
	{
		_CONNECTION_TAG * pTag = static_cast< _CONNECTION_TAG * >( pConnection->GetTag() );

		_cprint( "%s: Initializing a client session failed.[IP:%s/Account:%s/ErrorCode:%d/Detail:%s]\n", m_pImpl->GetName(), pConnection->GetPeerAddress().GetAddr(), ( pTag ) ? pTag->szAccountName : "<Unknown>", nError, ( !strlen( m_pImpl->GetErrorDescription() ) ? "No detail" : m_pImpl->GetErrorDescription() ) );
		FileLogHandler::GetFileLogHandler()->LogStringEx( NULL, "SecuritySolution", "%s: Initializing a client session failed.[IP:%s/Account:%s/ErrorCode:%d/Detail:%s]", m_pImpl->GetName(), pConnection->GetPeerAddress().GetAddr(), ( pTag ) ? pTag->szAccountName : "<Unknown>", nError, ( !strlen( m_pImpl->GetErrorDescription() ) ? "No detail" : m_pImpl->GetErrorDescription() ) );

		SendDisconnectDesc( pConnection, TS_SC_DISCONNECT_DESC::DISCONNECT_TYPE_ANTI_HACK );
		pConnection->Close();

		return false;
	}

	return true;
}

const bool XSecuritySolutionManager::BlockInitIncompleteClientSession( struct IStreamSocketConnection * pConnection )
{
	if( !m_pImpl )
		return false;

	if( isExceptionalIP( pConnection->GetPeerAddress().GetAddr() ) )
		return false;

	if( m_pImpl->IsInitCompletedClientSession( pConnection ) )
		return false;

	// 이 이하는 블록되는 조건에 해당된 유저들의 블록 처리

	_CONNECTION_TAG * pTag = static_cast< _CONNECTION_TAG * >( pConnection->GetTag() );

	_cprint( "%s: Not initialized client attempted to login.[IP:%s/Account:%s]\n", m_pImpl->GetName(), pConnection->GetPeerAddress().GetAddr(), ( pTag ) ? pTag->szAccountName : "<Unknown>" );
	FileLogHandler::GetFileLogHandler()->LogStringEx( NULL, "SecuritySolution", "%s: Not initialized client attempted to login.[IP:%s/Account:%s]", m_pImpl->GetName(), pConnection->GetPeerAddress().GetAddr(), ( pTag ) ? pTag->szAccountName : "<Unknown>" );

	if( GameRule::bUseAutoJail )
	{
		StructPlayer::AddToAutoAccountList( pTag->nAccountID );
	}

	StructPlayer * pPlayer = pTag->pPlayer;

	if( pPlayer )
		LOG::Log11N4S( LM_AUTO_USER_CHECKED, pPlayer->GetAccountID(), pPlayer->GetSID(), pPlayer->GetLevel(), pPlayer->GetJobLevel(), pPlayer->GetJobId(), 0, 0, 0, 0, AUTO_USER_CHECK_TYPE::BLOCKING_INIT_INCOMPLETE_CLIENT, 0, pPlayer->GetAccountName(), LOG::STR_NTS, pPlayer->GetName(), LOG::STR_NTS, "", 0, "", 0 );
	else
		LOG::Log11N4S( LM_AUTO_USER_CHECKED, pTag->nAccountID, 0, 0, 0, 0, 0, 0, 0, 0, AUTO_USER_CHECK_TYPE::BLOCKING_INIT_INCOMPLETE_CLIENT, 0, pTag->szAccountName, LOG::STR_NTS, "", 0, "", 0, "", 0 );

	SendDisconnectDesc( pConnection, TS_SC_DISCONNECT_DESC::DISCONNECT_TYPE_ANTI_HACK );
	pConnection->Close();

	return true;
}

const void XSecuritySolutionManager::DeinitClientSession( struct IStreamSocketConnection * pConnection )
{
	if( !m_pImpl )
		return;

	if( isExceptionalIP( pConnection->GetPeerAddress().GetAddr() ) )
		return;

	DWORD nError = m_pImpl->DeinitClientSession( pConnection );
	if( nError != SS_ERROR_SUCCESS && nError != SS_ERROR_NOT_INITIALIZED )
	{
		_CONNECTION_TAG * pTag = static_cast< _CONNECTION_TAG * >( pConnection->GetTag() );

		_cprint( "%s: Deinitializing a client session failed.[IP:%s/Account:%s/ErrorCode:%d/Detail:%s]\n", m_pImpl->GetName(), pConnection->GetPeerAddress().GetAddr(), ( pTag ) ? pTag->szAccountName : "<Unknown>", nError, ( !strlen( m_pImpl->GetErrorDescription() ) ? "No detail" : m_pImpl->GetErrorDescription() ) );
		FileLogHandler::GetFileLogHandler()->LogStringEx( NULL, "SecuritySolution", "%s: Deinitializing a client session failed.[IP:%s/Account:%s/ErrorCode:%d/Detail:%s]", m_pImpl->GetName(), pConnection->GetPeerAddress().GetAddr(), ( pTag ) ? pTag->szAccountName : "<Unknown>", nError, ( !strlen( m_pImpl->GetErrorDescription() ) ? "No detail" : m_pImpl->GetErrorDescription() ) );

		return;
	}

	return;
}

const bool XSecuritySolutionManager::ProcValidation( struct IStreamSocketConnection * pConnection )
{
	if( !m_pImpl )
		return true;

	if( isExceptionalIP( pConnection->GetPeerAddress().GetAddr() ) )
		return true;

	// 초기화가 완료되기 전에 들어오는 ProcValidation은 모두 성공으로 넘겨 줌
	if( !m_pImpl->IsInitCompletedClientSession( pConnection ) )
		return true;

	DWORD nError = m_pImpl->ProcValidation( pConnection );
	if( nError == SS_ERROR_SUCCESS )
		return true;

	// 이 이하는 오류 발생 시 처리 부
	_CONNECTION_TAG * pTag = static_cast< _CONNECTION_TAG * >( pConnection->GetTag() );

	_cprint( "%s: Validation failed.[IP:%s/Account:%s/ErrorCode:%d/Detail:%s]\n", m_pImpl->GetName(), pConnection->GetPeerAddress().GetAddr(), ( pTag ) ? pTag->szAccountName : "<Unknown>", nError, ( !strlen( m_pImpl->GetErrorDescription() ) ? "No detail" : m_pImpl->GetErrorDescription() ) );
	FileLogHandler::GetFileLogHandler()->LogStringEx( NULL, "SecuritySolution", "%s: Validation failed.[IP:%s/Account:%s/ErrorCode:%d/Detail:%s]", m_pImpl->GetName(), pConnection->GetPeerAddress().GetAddr(), ( pTag ) ? pTag->szAccountName : "<Unknown>", nError, ( !strlen( m_pImpl->GetErrorDescription() ) ? "No detail" : m_pImpl->GetErrorDescription() ) );

	switch( nError )
	{
	case SS_ERROR_CREATING_REQUEST_FAILED:
	case SS_ERROR_CLIENT_RESPONSE_TIMEOUT:
		SendDisconnectDesc( pConnection, TS_SC_DISCONNECT_DESC::DISCONNECT_TYPE_ANTI_HACK );
		pConnection->Close();
		break;

	default:
		break;
	}

	return false;
}

const bool XSecuritySolutionManager::OnValidationResponse( struct IStreamSocketConnection * pConnection, const void * pResponseBuffer, unsigned int size )
{
	if( !m_pImpl )
		return true;

	if( isExceptionalIP( pConnection->GetPeerAddress().GetAddr() ) )
		return true;

	_CONNECTION_TAG * pTag = static_cast< _CONNECTION_TAG * >( pConnection->GetTag() );
	DWORD nError = m_pImpl->OnValidationResponse( pConnection, pResponseBuffer, size );
	if( nError == SS_ERROR_SUCCESS )
		return true;

	// 이 이하는 오류 발생 시 처리 부
	//_CONNECTION_TAG * pTag = static_cast< _CONNECTION_TAG * >( pConnection->GetTag() );

	_cprint( "%s: Validation failed.[IP:%s/Account:%s/ErrorCode:%d/Detail:%s]\n", m_pImpl->GetName(), pConnection->GetPeerAddress().GetAddr(), ( pTag ) ? pTag->szAccountName : "<Unknown>", nError, ( !strlen( m_pImpl->GetErrorDescription() ) ? "No detail" : m_pImpl->GetErrorDescription() ) );
	FileLogHandler::GetFileLogHandler()->LogStringEx( NULL, "SecuritySolution", "%s: Validation failed.[IP:%s/Account:%s/ErrorCode:%d/Detail:%s]", m_pImpl->GetName(), pConnection->GetPeerAddress().GetAddr(), ( pTag ) ? pTag->szAccountName : "<Unknown>", nError, ( !strlen( m_pImpl->GetErrorDescription() ) ? "No detail" : m_pImpl->GetErrorDescription() ) );

	switch( nError )
	{
	case SS_ERROR_INVALID_CLIENT_RESPONSE:
		if( pTag->pPlayer )
		{
			StructPlayer * pPlayer = pTag->pPlayer;

			if( GameRule::bUseAutoJail )
			{
				// 여기선 지역 락을 걸 수 없으므로 AddState 같은 방송 동반 처리를 하면 안 됨
				pPlayer->SetAutoUsed();
				pPlayer->Save();
			}
			LOG::Log11N4S( LM_AUTO_USER_CHECKED, pPlayer->GetAccountID(), pPlayer->GetSID(), pPlayer->GetLevel(), pPlayer->GetJobLevel(), pPlayer->GetJobId(), 0, 0, 0, 0, AUTO_USER_CHECK_TYPE::INVALID_SECURITY_SOLUTION_RESPONSE, 0, pPlayer->GetAccountName(), LOG::STR_NTS, pPlayer->GetName(), LOG::STR_NTS, "", 0, "", 0 );
		}
		else
		{
			LOG::Log11N4S( LM_AUTO_USER_CHECKED, pTag->nAccountID, 0, 0, 0, 0, 0, 0, 0, 0, AUTO_USER_CHECK_TYPE::INVALID_SECURITY_SOLUTION_RESPONSE, 0, pTag->szAccountName, LOG::STR_NTS, "", 0, "", 0, "", 0 );
		}

		if( GameRule::bUseAutoJail )
		{
			StructPlayer::AddToAutoAccountList( pTag->nAccountID );
		}

		SendDisconnectDesc( pConnection, TS_SC_DISCONNECT_DESC::DISCONNECT_TYPE_ANTI_HACK );
		pConnection->Close();
		break;

	default:
		break;
	}

	return false;
}

const bool XSecuritySolutionManager::isExceptionalIP( const char * pszIP ) const
{
	if( GameRule::strSecuritySolutionExceptionalIP.empty() )
		return false;

	static std::string strPrevSecuritySolutionExceptionalIP = GameRule::strSecuritySolutionExceptionalIP;
	static std::vector< std::string > vToken;

	if( strPrevSecuritySolutionExceptionalIP != GameRule::strSecuritySolutionExceptionalIP )
	{
		strPrevSecuritySolutionExceptionalIP = GameRule::strSecuritySolutionExceptionalIP;
		XStringUtil::Split( GameRule::strSecuritySolutionExceptionalIP.c_str(), vToken, ";", false );
	}

	for( std::vector< std::string >::const_iterator it = vToken.begin() ; it != vToken.end() ; ++it )
	{
		if( nsl::wildcmp( (*it).c_str(), pszIP ) )
		{
			return true;
		}
	}

	return false;
}
