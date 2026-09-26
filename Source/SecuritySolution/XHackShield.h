#pragma once

#include "ISecuritySolutionImpl.h"
#include "ArTime.h"
#include "KHash.h"

#include <hack_shield/AntiCpXSvr.h>


struct XHackShield : public ISecuritySolutionImpl
{
public:
	enum
	{
		MAX_CLIENT_RESPONSE_TIMEOUT_COUNT		= 3,
	};

public:
	XHackShield()
		: ISecuritySolutionImpl( "HackShield" )
		, m_hServer( ANTICPX_INVALID_HANDLE_VALUE )
	{}

	virtual const DWORD			Init();
	virtual const DWORD			DeInit();

	virtual const DWORD			InitClientSession( struct IStreamSocketConnection * pConnection );
	virtual const bool			IsInitCompletedClientSession( struct IStreamSocketConnection * pConnection ) const;
	virtual const DWORD			DeinitClientSession( struct IStreamSocketConnection * pConnection );
	virtual const DWORD			ProcValidation( struct IStreamSocketConnection * pConnection );
	virtual const DWORD			OnValidationResponse( struct IStreamSocketConnection * pConnection, const void * pResponseBuffer, unsigned int size );

private:
	AHNHS_SERVER_HANDLE				m_hServer;

	// m_hsSecurityResponseTimeoutCount와 m_nNextTimeoutResetTime는 g_ConnectionTagLock이 걸린 채로 호출되는 함수에서만 사용되므로 별도로 락을 걸지 않음
	KHash< int, hashPr_mod_int >	m_hsResponseTimeoutCount;
	AR_TIME							m_nNextTimeoutResetTime;

	struct ClientSessionInfo
	{
		ClientSessionInfo()
			: m_eStatus( STATUS_IDLE )	// 초기화가 되지 않으면 구조체의 인스턴스를 생성하지 않으므로 기본은 IDLE
			, m_bInitComplete( false )
			, m_bNeedNewRequestMsg( true )
			, m_hClientSession( ANTICPX_INVALID_HANDLE_VALUE )
			, m_nNextRequestTime( GetArTime() )
		{
			memset( &m_RequestBuffer, 0, sizeof( m_RequestBuffer ) );
		}

		_CLIENT_STATUS				m_eStatus;
		bool						m_bInitComplete;
		bool						m_bNeedNewRequestMsg;
		AHNHS_CLIENT_HANDLE			m_hClientSession;
		AHNHS_TRANS_BUFFER			m_RequestBuffer;
		AR_TIME						m_nNextRequestTime;
	};
};
