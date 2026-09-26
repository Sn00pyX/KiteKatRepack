#pragma once

#include <string>
#include <windows.h>


enum _ERROR_CODES
{
	SS_ERROR_SUCCESS					= 0,

	SS_ERROR_INIT_FAILED				= 1,
	SS_ERROR_ALREADY_INITIALIZED		= 2,
	SS_ERROR_NOT_INITIALIZED			= 3,
	SS_ERROR_NO_CONNECTION_TAG			= 4,
	SS_ERROR_CREATING_REQUEST_FAILED	= 5,
	SS_ERROR_CLIENT_RESPONSE_TIMEOUT	= 6,
	SS_ERROR_INVALID_CLIENT_RESPONSE	= 7,
};

struct ISecuritySolutionImpl
{
public:
	ISecuritySolutionImpl( const char * pszSolutionName )
		: m_bInit( false )
		, m_strSolutionName( pszSolutionName )
	{}
	virtual ~ISecuritySolutionImpl()		{}

protected:
	enum _CLIENT_STATUS
	{
		STATUS_NOT_INITIALIZED				= 0,	// 초기화 전

		// 서버에서 클라로 Query 후 클라에서 서버로 Response 구조의 보안 솔루션 구조용 STATUS 상수들
		STATUS_IDLE							= 100,	// 쉬는 중
		STATUS_WAITING_RESPONSE				= 101,	// 클라에 확인 요청 보내고 응답 기다리는 중
	};

public:
	virtual const DWORD			Init()							= 0;
	virtual const DWORD			DeInit()						= 0;
	const bool					IsInitialized() const			{ return m_bInit; }
	const char *				GetName() const					{ return m_strSolutionName.c_str(); }

	// 이하 5개의 함수는 g_ConnectionTagLock이 걸린 채로 사용되어야 함
	virtual const DWORD			InitClientSession( struct IStreamSocketConnection * pConnection )									= 0;
	virtual const bool			IsInitCompletedClientSession( struct IStreamSocketConnection * pConnection ) const					= 0;
	virtual const DWORD			DeinitClientSession( struct IStreamSocketConnection * pConnection )									= 0;
	virtual const DWORD			ProcValidation( struct IStreamSocketConnection * pConnection )										= 0;
	virtual const DWORD			OnValidationResponse( struct IStreamSocketConnection * pConnection, const void * pResponseBuffer, unsigned int size )	= 0;

	const char *				GetErrorDescription() const		{ return m_strErrorDescription.c_str(); }

protected:
	bool					m_bInit;
	std::string				m_strSolutionName;
	mutable std::string		m_strErrorDescription;
};
