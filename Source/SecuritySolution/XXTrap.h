#pragma once

#include <mmo/ArTime.h>
#include <toolkit/KHash.h>

#include "ISecuritySolutionImpl.h"

#include <xtrap/Xtrap_S_Interface.h>


struct XXTrap : public ISecuritySolutionImpl
{
public:
	enum
	{
		MAX_CLIENT_RESPONSE_TIMEOUT_COUNT		= 3,

		XTRAP_CS4_MAP_SIZE						= 13000,
		XTRAP_CLIENT_SESSION_BUFFER_SIZE		= 320,
		XTRAP_CLIENT_TRANSFER_BUFFER_SIZE		= 128,
	};

public:
	XXTrap()
		: ISecuritySolutionImpl( "X-Trap" )
		, m_pszXTrapMap( 0 )
		, m_nMapCount( 0 )
	{}
	virtual ~XXTrap()
	{
		if( m_pszXTrapMap )
			delete [] m_pszXTrapMap;
	}

	virtual const DWORD			Init();
	virtual const DWORD			DeInit();

	virtual const DWORD			InitClientSession( struct IStreamSocketConnection * pConnection );
	virtual const bool			IsInitCompletedClientSession( struct IStreamSocketConnection * pConnection ) const;
	virtual const DWORD			DeinitClientSession( struct IStreamSocketConnection * pConnection );
	virtual const DWORD			ProcValidation( struct IStreamSocketConnection * pConnection );
	virtual const DWORD			OnValidationResponse( struct IStreamSocketConnection * pConnection, const void * pResponseBuffer, unsigned int size );

protected:
	const char *				getErrorDescription( const unsigned int nError ) const;

private:
	unsigned char *				m_pszXTrapMap;
	size_t						m_nMapCount;

	// m_hsSecurityResponseTimeoutCount와 m_nNextTimeoutResetTime는 g_ConnectionTagLock이 걸린 채로 호출되는 함수에서만 사용되므로 별도로 락을 걸지 않음
	KHash< int, hashPr_mod_int >	m_hsResponseTimeoutCount;
	AR_TIME							m_nNextTimeoutResetTime;

	struct ClientSessionInfo
	{
		ClientSessionInfo()
			: m_eStatus( STATUS_IDLE )	// 초기화가 되지 않으면 구조체의 인스턴스를 생성하지 않으므로 기본은 IDLE
			, m_bInitComplete( false )
			, m_nNextRequestTime( GetArTime() )
		{
			memset( m_szClientSession, 0, sizeof( m_szClientSession ) );
		}

		_CLIENT_STATUS				m_eStatus;
		bool						m_bInitComplete;
		unsigned char				m_szClientSession[ XTRAP_CLIENT_SESSION_BUFFER_SIZE ];
		AR_TIME						m_nNextRequestTime;
	};
};
