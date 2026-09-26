#pragma once


#ifdef	USE_GAME_GUARD_AUTH_3

#include <mmo/ArTime.h>
#include <toolkit/KHash.h>

#include "ISecuritySolutionImpl.h"

#include <game_guard/ggsrv30.h>


struct XGameGuard3 : public ISecuritySolutionImpl
{
public:
	XGameGuard3()
		: ISecuritySolutionImpl( "GameGuard3" )
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
			: m_pCCSAuth3( NULL )
			, m_bIsFirstTimeCheck( false )
			, m_nNextRequestTime( GetArTime() )
		{
			m_pCCSAuth3 = new CCSAuth3();
			m_pCCSAuth3->Init( true );
		}

		~ClientSessionInfo()
		{
			m_pCCSAuth3->Close();
			delete m_pCCSAuth3;
		}

		CCSAuth3 *	m_pCCSAuth3;
		bool		m_bIsFirstTimeCheck;
		AR_TIME		m_nNextRequestTime;
	};


private:
	const DWORD					sendAuthQuery( struct IStreamSocketConnection * pConnection, struct ClientSessionInfo * pSession );

};

#endif
