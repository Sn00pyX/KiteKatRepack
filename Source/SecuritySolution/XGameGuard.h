#pragma once

#ifndef	USE_GAME_GUARD_AUTH_3

#include <mmo/ArTime.h>
#include <toolkit/KHash.h>

#include "ISecuritySolutionImpl.h"

#ifdef _WIN64
#include <game_guard/x64/ggsrv25.h>
#else
#include <game_guard/ggsrv25.h>
#endif


struct XGameGuard : public ISecuritySolutionImpl
{
public:
	enum
	{
		MAX_CLIENT_RESPONSE_TIMEOUT_COUNT		= 3,
	};

	static const AR_TIME		INITIAL_VALIDATION_TIMEOUT		= 6000;

public:
	XGameGuard()
		: ISecuritySolutionImpl( "GameGuard" )
	{}

	virtual const DWORD			Init();
	virtual const DWORD			DeInit();

	virtual const DWORD			InitClientSession( struct IStreamSocketConnection * pConnection );
	virtual const bool			IsInitCompletedClientSession( struct IStreamSocketConnection * pConnection ) const;
	virtual const DWORD			DeinitClientSession( struct IStreamSocketConnection * pConnection );
	virtual const DWORD			ProcValidation( struct IStreamSocketConnection * pConnection );
	virtual const DWORD			OnValidationResponse( struct IStreamSocketConnection * pConnection, const void * pResponseBuffer, unsigned int size );

private:
	struct ClientSessionInfo
	{
		ClientSessionInfo()
			: m_eStatus( STATUS_IDLE )	// 초기화가 되지 않으면 구조체의 인스턴스를 생성하지 않으므로 기본은 IDLE
			, m_bInitComplete( false )
			, m_bNeedNewRequestMsg( true )
			, m_pCCSAuth2( NULL )
			, m_nNextRequestTime( GetArTime() )
		{
			m_pCCSAuth2 = new CCSAuth2();
			m_pCCSAuth2->Init();
		}

		~ClientSessionInfo()
		{
			delete m_pCCSAuth2;
		}

		_CLIENT_STATUS				m_eStatus;
		bool						m_bInitComplete;
		bool						m_bNeedNewRequestMsg;
		CCSAuth2 *					m_pCCSAuth2;
		AR_TIME						m_nNextRequestTime;
	};


private:
	const DWORD					sendAuthQuery( struct IStreamSocketConnection * pConnection, struct ClientSessionInfo * pSession );

private:
	// m_hsSecurityResponseTimeoutCount와 m_nNextTimeoutResetTime는 g_ConnectionTagLock이 걸린 채로 호출되는 함수에서만 사용되므로 별도로 락을 걸지 않음
	KHash< int, hashPr_mod_int >	m_hsResponseTimeoutCount;
	AR_TIME							m_nNextTimeoutResetTime;
};


#endif
