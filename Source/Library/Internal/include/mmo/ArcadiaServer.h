#pragma once

#include <vector>

#include "ArObject.h"
#include "ArOption.h"

#include "../toolkit/XConsole.h"
#include "../toolkit/SafeTickCount.h"


/*

	ArcadiaServer 의 주요 기능은 다음과 같다.
	
	. 맵상의 object 들의 이동처리
	. 임의의 지역에 대한 lock (SMP 관련)

	Arcadia 엔진은 기본적으로 Network I/O 에 사용되는 메세지 프로토콜을 정의하지 않는다.
	Arcadia 는 메세지가 포함해야할 기본적인 정보들만을 노출시키고 있으며 그것의 실제적인
	구현은 전적으로 사용자에게 달려 있다.
	이를테면 "플레이어가 x,y 좌표로 이동한다" 를 다루기 위해 필요한 것은
	"ARCADIA_MOVE_MESSAGE" 같은 메세지 스트럭쳐가 아니라 단지 x, y 좌표일 뿐이다.

*/


struct ArcadiaIntf
{
	virtual void SendEnterMessage( const ArObject * pClient, const ArObject * pObj )	= 0;
	virtual void SendLeaveMessage( const ArObject * pClient, const ArObject * pObj )	= 0;
	virtual void SendForceMoveMessage( const ArObject * pClient, const ArObject * pObj )	= 0;
	virtual void SendMoveMessage( const ArObject * pClient, const ArObject * pObj )	= 0;
	virtual void SendMoveAckMessage( const ArObject * pClient, AR_TIME tm, unsigned char speed )	= 0;
	virtual void SendRegionAckMessage( const ArObject * pClient, unsigned rx, unsigned ry )			= 0;
	
	virtual void SendGameMessage( const ArObject * pClient, const void *Message )		= 0;

	virtual bool onSetMove( ArObject * pObj, const ArPosition & oldPos, const ArPosition & newPos )	{ return true; }

	virtual void onNearRegion( ArObject * pClient, ArObject * pObj )			{}
};

struct ArcadiaLock
{
	int					handle;
	
	ArcadiaLock( int _handle ) : handle( _handle )
	{
	}
};

class ArcadiaServer
{
public:
	
	virtual ~ArcadiaServer();

	bool Init( ArcadiaIntf * pIntf, AR_UNIT map_width, AR_UNIT map_height, void (*scheduler_thread_init_func)( int ), bool bMonitoringThread = false );
	bool DeInit();

	// priority 를 변경할 필요가 없을경우, call instruction 을 피해가기 위해 인라인으로 뽑았음.
	inline void SetObjectPriority( ArSchedulerObject * obj, ArSchedulerObject::AR_OBJECT_PRIORITY priority )
	{	
		// 이미 등록되어 있으면 무시~
		if( obj->GetPriority() == priority &&
			obj->GetPendingPriority() == -1 )			return;

		if( obj->GetPendingPriority() == priority )		return;
		
		setObjectPriority( obj, priority );
	}

	// DeleteObject 호출시 자동으로 스케쥴러에서 제거되고, 이후 적절한 때에 ProcDelete() 가 호출 된다.
	void		DeleteObject( ArSchedulerObject* pObject );

	void		onRegionChange( ArObject* pObject, AR_TIME update_time, bool bIsStopMessage );

	// 이하 함수들에서 layer 관련 변수가 short 타입인 이유는 unsigned char 타입에서 -1과 255가 같은 값으로 취급되는 문제를 피하기 위함
	ArcadiaLock	_LockWorld( const char * filename, int linenumber );
	ArcadiaLock	_LockWithVisibleRange( const char * filename, int linenumber, unsigned rx, unsigned ry, short layer = -1 );
	ArcadiaLock	_LockObjectWithVisibleRange( const char * filename, int linenumber, const ArObject * pObject );
	ArcadiaLock	_LockObjectWithSpecificRegion( const char * filename, int linenumber, const ArObject * pObject, unsigned rx, unsigned ry, short layer = -2 );
	ArcadiaLock	_LockObject( const char * filename, int linenumber, const ArObject * pObject );
	ArcadiaLock _LockObjects( const char * filename, int linenumber, const ArObject * pObject1, const ArObject * pObject2 );
	ArcadiaLock	_LockArea( const char * filename, int linenumber, unsigned rx1, unsigned ry1, unsigned rx2, unsigned ry2, short layer = -1 );	// region_update 로만 사용해야함
	ArcadiaLock	_LockObjectWithSpecificRange( const char * filename, int linenumber, const ArObject *pObject, unsigned range );

	void		UnLock( ArcadiaLock * pLockHandle );

	const bool	IsLocked( const unsigned int rx, const unsigned int ry, const short layer = -1, const bool bPartial = false );
	const bool	IsLocked( unsigned rx1, unsigned ry1, unsigned rx2, unsigned ry2, const short layer = -1, const bool bPartial = false );
	const bool	IsLocked( const ArObject *pObject, const bool bPartial = false );

	unsigned	Broadcast( unsigned rx, unsigned ry, unsigned char layer, const void * msg );
	unsigned	Broadcast( unsigned rx1, unsigned ry1, unsigned rx2, unsigned ry2, unsigned char layer, const void * msg );
	unsigned	BroadcastToSpecificRegion( unsigned rx, unsigned ry, unsigned range, unsigned char layer, const void * msg );

	// Lock 을 해야함.
	void AddObject( ArObject* pObject );
	void RemoveObject( ArObject* pObject );
	void EnumMovableObject( const ArPosition & pos, unsigned char layer, AR_UNIT range, std::vector< AR_HANDLE > * pvResult, bool bIncludeClient = true, bool bIncludeNPC = true );
	void EnumMovableObject( int rx1, int ry1, int rx2, int ry2, unsigned char layer, std::vector< AR_HANDLE > * pvResult, bool bIncludeClient = true, bool bIncludeNPC = true );
	void EnumMovableObjectInEveryRegion( int rx1, int ry1, int rx2, int ry2, unsigned char layer, std::vector< AR_HANDLE > * pvResult, bool bIncludeClient = true, bool bIncludeNPC = true );
	void DoEachVisibleRegion( int rx, int ry, unsigned char layer, struct ArRegionFunctor & _fo );
	void DoEachVisibleRegion( int rx1, int ry1, int rx2, int ry2, unsigned char layer, ArRegionFunctor & _fo );
	void DoEachRegion( int rx1, int ry1, int rx2, int ry2, unsigned char layer, ArRegionFunctor & _fo );

	bool SetMultipleMove( ArObject* pObject, const ArPosition & curPos, const std::vector< ArPosition > & newPos, unsigned char speed, bool bAbsoluteMove, AR_TIME t, bool bBroadcastMove = true );
	bool SetMove( ArObject* pObject, const ArPosition & curPos, const ArPosition & newPos, unsigned char speed, bool bAbsoluteMove, AR_TIME t, bool bBroadcastMove = true );
	void MoveObject( ArObject* pObject, const ArPosition & newPos, const float face );

	static struct ArcadiaIntf* GetIntf()	{ return m_pIntf; }
	
	LONG	GetLockedThreadCount();
	LONG	GetWaitingThreadCount();

	const LONG & GetUserCount()				{ return m_nUserCount; }
	const LONG & GetNPCCount()				{ return m_nNPCCount; }
	const LONG & GetItemCount()				{ return m_nItemCount; }

	static ArcadiaServer & Instance();

protected:

private:

	int arcadia_lock( struct ArBoxCollision * pLockInfo, const char * filename, int linenumber );

	void setObjectPriority( ArSchedulerObject * pObj, ArSchedulerObject::AR_OBJECT_PRIORITY priority );

	ArcadiaServer();

	void	step( ArObject* pObject, AR_TIME tm );
	void	onMoveObject( ArObject* pObject, const ArPosition & oldPos, const ArPosition & newPos );
	void	enterProc( ArObject* pObject, unsigned prx, unsigned pry );

	static struct ArcadiaIntf*	m_pIntf;
	static struct ArcadiaData*	m_pData;

	AR_UNIT m_nMapWidth, m_nMapHeight;

	LONG m_nUserCount;
	LONG m_nNPCCount;
	LONG m_nItemCount;
};


struct ArcadiaAutoLock
{
	ArcadiaAutoLock()
		: plockhandle(NULL)
		, lockHandle(0)
		, tLockStartTick( GetSafeTickCount() )
		, szFileName( "??" )
		, nLineNumber( 0 )
	{
	}
	ArcadiaAutoLock( ArcadiaLock & lock, const char *fn = "??", int ln = 0 )
		: lockHandle( lock.handle )
		, szFileName( fn )
		, nLineNumber( ln )
		, tLockStartTick( GetSafeTickCount() )
	{
		plockhandle = &lockHandle;
	}

	~ArcadiaAutoLock()
	{
		unsigned int lock_time = GetSafeTickCount() - tLockStartTick;

		if( plockhandle ) ArcadiaServer::Instance().UnLock( plockhandle );
		extern volatile int g_bUseLockDelayLogging;
		if( !g_bUseLockDelayLogging )
			return;

		if( lock_time > 1000 )
		{
			// 로그를 남기자..
			_lprint( "LockPerfLog.txt", "%s : %d [%d]\n", szFileName, nLineNumber, lock_time );
		}
	}

	void set( ArcadiaLock & lock, const char *fn = "??", int ln = 0 )
	{
		szFileName = fn;
		nLineNumber = ln;
		tLockStartTick = GetSafeTickCount();
		lockHandle.handle = lock.handle;
		plockhandle = &lockHandle;
	}

	const char*		szFileName;
	int				nLineNumber;
	unsigned int	tLockStartTick;
	ArcadiaLock*	plockhandle;
	ArcadiaLock		lockHandle;
};


// __VA_ARGS__ 관련 내용은 http://msdn.microsoft.com/en-us/library/ms177415(VS.80).aspx 참조(가변 파라미터 매크로 정의)
#define LockWorld() _LockWorld( __FILE__, __LINE__ )
// Syntax: Lock( rx, ry[, layer] )
#define LockWithVisibleRange( rx, ry, ... ) _LockWithVisibleRange( __FILE__, __LINE__, rx, ry, __VA_ARGS__ )
// Syntax: LockArea( rx1, ry1, rx2, ry2[, layer] )
#define LockArea( rx1, ry1, rx2, ry2, ... ) _LockArea( __FILE__, __LINE__, rx1, ry1, rx2, ry2, __VA_ARGS__ )
// Syntax: LockObjectWithSpecificRegion( pObject, rx, ry[, layer] )
#define LockObjectWithSpecificRegion( pObject, rx, ry, ... ) _LockObjectWithSpecificRegion( __FILE__, __LINE__, pObject, rx, ry, __VA_ARGS__ )
#define LockObjectWithVisibleRange( pObject ) _LockObjectWithVisibleRange( __FILE__, __LINE__, pObject )
#define LockObject( pObject ) _LockObject( __FILE__, __LINE__, pObject )
#define LockObjects( pObject1, pObject2 ) _LockObjects( __FILE__, __LINE__, pObject1, pObject2 )
#define LockObjectWithSpecificRange( pObject, range ) _LockObjectWithSpecificRange( __FILE__, __LINE__, pObject, range )

#define ARCADIA_LOCK(x)		ArcadiaAutoLock _lock( x, __FILE__, __LINE__ )
