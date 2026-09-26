#pragma once


// ISecuritySolutionImpl을 상속받은 녀석의 인스턴스 관리 및 예외 IP 체크만 함
struct XSecuritySolutionManager
{
public:
	enum _SECURITY_SOLUTION_TYPE
	{
		TYPE_NONE			= 0,
		TYPE_GAMEGUARD		= 1,
		TYPE_HACKSHIELD		= 2,
		TYPE_XTRAP			= 3,
	};
public:
	static XSecuritySolutionManager &	Instance()
	{
		static XSecuritySolutionManager _instance;
		return _instance;
	}
	~XSecuritySolutionManager();

	const bool			Init();
	const void			Deinit();

	// 이하 5개의 함수는 g_ConnectionTagLock이 걸린 채로 사용되어야 함
	const bool			InitClientSession( struct IStreamSocketConnection * pConnection );
	const bool			BlockInitIncompleteClientSession( struct IStreamSocketConnection * pConnection );
	const void			DeinitClientSession( struct IStreamSocketConnection * pConnection );
	const bool			ProcValidation( struct IStreamSocketConnection * pConnection );
	const bool			OnValidationResponse( struct IStreamSocketConnection * pConnection, const void * pResponseBuffer, unsigned int size );

protected:
	XSecuritySolutionManager()
		: m_pImpl( 0 )
	{}

	const bool			isExceptionalIP( const char * pszIP ) const;

protected:
	struct ISecuritySolutionImpl *		m_pImpl;
};
