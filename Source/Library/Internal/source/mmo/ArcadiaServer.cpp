
#include "../../include/toolkit/ExclusiveLogicLock.h"
#include "../../include/toolkit/ILock.h"

#include "../../include/toolkit/XEnv.h"

#include "../../include/mmo/ArcadiaServer.h"
#include "../../include/mmo/ArRegion.h"
#include "../../include/mmo/ArBoxCollision.h"
#include "../../include/mmo/ArScheduler.h"


ArcadiaServer * s_pArcadiaServer;


__declspec( thread ) bool			s_LockFlag = false;

struct ArcadiaData
{
	~ArcadiaData()
	{
		delete pRgnMgr;
	}

	ArRegionContainer	*pRgnMgr;
	ExclusiveLogicLock< ArBoxCollision, XCriticalSection > rgnLock;

	ArScheduler			scheduler;
};

ArcadiaServer::ArcadiaServer()
{	
	m_pData = NULL;
	m_pIntf = NULL;
	s_pArcadiaServer = this;
}

ArcadiaServer::~ArcadiaServer()
{
	DeInit();
}

ArcadiaServer & ArcadiaServer::Instance()
{	
	static ArcadiaServer inst;
	return inst;
}

ArcadiaIntf* ArcadiaServer::m_pIntf;
ArcadiaData* ArcadiaServer::m_pData;

static int _getArTime()
{
	return GetArTime();
}

bool ArcadiaServer::Init( ArcadiaIntf *pIntf, AR_UNIT map_width, AR_UNIT map_height, void (*scheduler_thread_init_func)( int ), bool bMonitoringThread )
{
	m_nUserCount = 0;
	m_nNPCCount = 0;
	m_nItemCount = 0;

	m_nMapWidth = map_width;
	m_nMapHeight = map_height;

	m_pIntf = pIntf;

	g_nRegionSize = ENV().GetInt( "engine.region_size", 180 );
	ENV().Set(  "engine.region_size",	g_nRegionSize );

	m_pData = new ArcadiaData;

	if( !m_pData->scheduler.Init( ENV().GetInt( "engine.scheduler_count", 4 ), scheduler_thread_init_func, bMonitoringThread ) )
	{
		delete m_pData;
		return false;
	}

	m_pData->pRgnMgr = new ArRegionContainer( m_nMapWidth, m_nMapHeight );

	ENV().Bind( "engine.lock", (int*)&m_pData->rgnLock.GetLockedThreadCount() );
	ENV().Bind( "engine.wait", (int*)&m_pData->rgnLock.GetWaitingThreadCount() );

	ENV().Bind( "engine.count_client", (int*)&GetUserCount() );
	ENV().Bind( "engine.count_movable", (int*)&GetNPCCount() );
	ENV().Bind( "engine.count_static", (int*)&GetItemCount() );

	ENV().Bind( "engine.ar_time", _getArTime );

	ENV().Bind( "engine.use_region_debug", (int*)&g_bUseRegionDebug );

	return true;
}

bool ArcadiaServer::DeInit()
{
	if( m_pData )
	{
		m_pData->scheduler.DeInit();
		delete m_pData;
	}
	m_pData = NULL;

	return true;
}

LONG ArcadiaServer::GetLockedThreadCount()
{
	return m_pData->rgnLock.GetLockedThreadCount();
};

LONG ArcadiaServer::GetWaitingThreadCount()
{
	return m_pData->rgnLock.GetWaitingThreadCount();
}

void sendEnterMessage( const ArObject * _pClient, ArObject * pObject, bool bAbsolute = false )
{
	// 만약 pClient 의 이전 관심영역에 있는거라면 무시한다.
	const ArObject * pClient = static_cast< const ArObject * >( _pClient );
	
	if( pObject->GetObjectType() == ArObject::MOVABLE_OBJECT )
	{
		//  pClient 에게 보여지고 있는 것이므로 pObject 는 활성화된다.
		pObject->bIsNearClient = true;

		// _oprint( "ADD BY SERVER : %I64d\n", pObject->GetHandle() );


		if( pObject->GetFinalPriority() < ArObject::UPDATE_PRIORITY_NORMAL )
			ArcadiaServer::Instance().SetObjectPriority( pObject, ArObject::UPDATE_PRIORITY_NORMAL );
	}

	ArcadiaServer::GetIntf()->SendEnterMessage( pClient, pObject );
}

// 강제 이동 메시지
struct _SendForceMoveMessage : ArObjectFunctor
{
	_SendForceMoveMessage( const ArObject * _pWho )
	{
		pWho = _pWho;
	}

	virtual void operator()( ArObject * pClient ) const
	{
		//if( pClient == pWho ) return;

		ArcadiaServer::GetIntf()->SendForceMoveMessage( pClient, static_cast< const ArObject* >( pWho ) );
	}

	const ArObject * pWho;
};

// 해당 리전의 녀석들에게 보낼때
struct _SendMoveMessage : ArObjectFunctor
{
	_SendMoveMessage( const ArObject * _pWho )
	{
		pWho = _pWho;
	}

	virtual void operator()( ArObject * pClient ) const
	{
		//if( pClient == pWho ) return;

		ArcadiaServer::GetIntf()->SendMoveMessage( pClient, static_cast< const ArObject* >( pWho ) );
	}

	const ArObject * pWho;
};

void ArcadiaServer::MoveObject( ArObject* pObject, const ArPosition & newPos, const float face )
{
	AR_TIME tm = GetArTime();

	ArPosition oldPos = pObject->GetPos();

	unsigned rx = GetRegionX( newPos.x );
	unsigned ry = GetRegionY( newPos.y );

	unsigned prx = pObject->GetRX();
	unsigned pry = pObject->GetRY();

	pObject->SetCurrentXY( newPos.x, newPos.y );
	pObject->SetFace( face );
	pObject->StopMove();

	ArPosition _newPos = newPos;

	onMoveObject( pObject, oldPos, _newPos );
	
	pObject->lastStepTime = tm;

	pObject->bIsRegionChanging = true;

	if( prx != rx || pry != ry )
	{
		enterProc( pObject, prx, pry );

		pObject->prev_rx = prx;
		pObject->prev_ry = pry;
	}

	pObject->bIsRegionChanging = false;

	struct _ArRegionFunctor : ArRegionFunctor
	{
		_ArRegionFunctor( const ArObject * _pObject )
		{
			nCnt = 0;
			pObject = _pObject;
		}

		void operator()( const ArRegion * pRegion )
		{
			_SendForceMoveMessage func( pObject );
			nCnt += pRegion->DoEachClient( func );
		}

		unsigned nCnt;
		const ArObject *pObject;
	} _rf( pObject );

	// 근처에 이동 메세지 전송
	m_pData->pRgnMgr->DoEachVisibleRegion( pObject->GetRX(), pObject->GetRY(), pObject->GetLayer(), _rf );
}

bool ArcadiaServer::SetMultipleMove( ArObject* pObject, const ArPosition & curPos, const std::vector< ArPosition > & newPos, unsigned char speed, bool bAbsoluteMove, AR_TIME t, bool bBroadcastMove )
{
	assert( newPos.size() );

	if( !bAbsoluteMove && !m_pIntf->onSetMove( pObject, curPos, newPos.back() ) ) return false;

	{
		const ArPosition oldPos = pObject->GetPos();
		pObject->SetCurrentXY( curPos.x, curPos.y );
		const ArPosition newPos = pObject->GetPos();
		onMoveObject( pObject, oldPos, newPos );

		enterProc( pObject, oldPos.GetRX(), oldPos.GetRY() );
	}


	pObject->SetMultipleMove( newPos, speed, t );

	if( !bBroadcastMove )
		return true;

	struct _ArRegionFunctor : ArRegionFunctor
	{
		_ArRegionFunctor( const ArObject * _pObject )
		{
			nCnt = 0;
			pObject = _pObject;
		}

		void operator()( const ArRegion * pRegion )
		{
			_SendMoveMessage func( pObject );
			nCnt += pRegion->DoEachClient( func );
		}

		unsigned nCnt;
		const ArObject *pObject;
	} _rf( pObject );

	// 근처에 이동 메세지 전송
	m_pData->pRgnMgr->DoEachVisibleRegion( pObject->GetRX(), pObject->GetRY(), pObject->GetLayer(), _rf );

	// 플레이어에게 요청한 이동 메세지가 처리되었음을 알려주자
	if( pObject->GetObjectType() == ArObject::CLIENT_OBJECT ) 
	{
		//m_pIntf->SendMoveAckMessage( pObject, tm, speed );
	}
	else if( pObject->GetObjectType() == ArObject::MOVABLE_OBJECT ) 
	{
		// { 만약 NPC 가 이동한 것인데 아무에게도 메세지가 전송되지 않았다면
		//   명백히 근처에 클라이언트가 없는 것이다.
		if( _rf.nCnt )
		{
			pObject->bIsNearClient = true;
			if( pObject->GetPriority() == ArObject::UPDATE_PRIORITY_NULL ||
				pObject->GetPriority() == ArObject::UPDATE_PRIORITY_IDLE )
			{
				SetObjectPriority( pObject, ArObject::UPDATE_PRIORITY_NORMAL );
			}
		}
		else
		{
			pObject->bIsNearClient = false;
		}
		// }
	}

	return true;
}

bool ArcadiaServer::SetMove( ArObject* pObject, const ArPosition & curPos, const ArPosition & newPos, unsigned char speed, bool bAbsoluteMove, AR_TIME t, bool bBroadcastMove )
{
	if( !bAbsoluteMove && !m_pIntf->onSetMove( pObject, curPos, newPos ) ) return false;

	if( pObject->IsMoving() )
	{
		{
			const ArPosition oldPos = pObject->GetPos();
			pObject->SetCurrentXY( curPos.x, curPos.y );
			const ArPosition newPos = pObject->GetPos();
			onMoveObject( pObject, oldPos, newPos );

			enterProc( pObject, oldPos.GetRX(), oldPos.GetRY() );
		}

		pObject->SetMove( newPos, speed, t );

	} else pObject->SetMove( newPos, speed, t );

	if( !bBroadcastMove )
		return true;

	struct _ArRegionFunctor : ArRegionFunctor
	{
		_ArRegionFunctor( const ArObject * _pObject )
		{
			nCnt = 0;
			pObject = _pObject;
		}

		void operator()( const ArRegion * pRegion )
		{
			_SendMoveMessage func( pObject );
			nCnt += pRegion->DoEachClient( func );
		}

		unsigned nCnt;
		const ArObject *pObject;
	} _rf( pObject );

	// 근처에 이동 메세지 전송
	m_pData->pRgnMgr->DoEachVisibleRegion( pObject->GetRX(), pObject->GetRY(), pObject->GetLayer(), _rf );

	// 플레이어에게 요청한 이동 메세지가 처리되었음을 알려주자
	if( pObject->GetObjectType() == ArObject::CLIENT_OBJECT ) 
	{
		//m_pIntf->SendMoveAckMessage( pObject, tm, speed );
	}
	else if( pObject->GetObjectType() == ArObject::MOVABLE_OBJECT ) 
	{
		// { 만약 NPC 가 이동한 것인데 아무에게도 메세지가 전송되지 않았다면
		//   명백히 근처에 클라이언트가 없는 것이다.
		if( _rf.nCnt )
		{
			pObject->bIsNearClient = true;
			if( pObject->GetPriority() == ArObject::UPDATE_PRIORITY_NULL ||
				pObject->GetPriority() == ArObject::UPDATE_PRIORITY_IDLE )
			{
				SetObjectPriority( pObject, ArObject::UPDATE_PRIORITY_NORMAL );
			}
		}
		else
		{
			pObject->bIsNearClient = false;
		}
		// }
	}

	return true;
}

void ArcadiaServer::setObjectPriority( ArSchedulerObject * pObj, ArSchedulerObject::AR_OBJECT_PRIORITY priority )
{
	if( !m_pData )
		return;

	m_pData->scheduler.SetObjectPriority( pObj, priority );
}

void ArcadiaServer::onRegionChange( ArObject* pObject, AR_TIME update_time, bool bIsStopMessage )
{
	unsigned prx = pObject->GetRX();
	unsigned pry = pObject->GetRY();

	pObject->bIsRegionChanging = true;

	AR_TIME tc = ( bIsStopMessage ? 10 : 0 );
	step( pObject,  pObject->lastStepTime + update_time + tc );

	// prev_rx, prev_ry 변경
	if( pObject->GetRX() != prx || pObject->GetRY() != pry )
	{
		enterProc( pObject, prx, pry );

		pObject->prev_rx = prx;
		pObject->prev_ry = pry;
	}

	pObject->bIsRegionChanging = false;
}

int ArcadiaServer::arcadia_lock( struct ArBoxCollision * pLockInfo, const char * filename, int linenumber )
{
	/*
	if( s_LockFlag ) 
	{
		assert( 0 );
		_cprint( "DOUBLE LOCK!\n" );
		FILE *fp = fopen( "DOUBLE_LOCK.txt", "a+" );
		if( fp ) fclose( fp );
	}
	*/

	s_LockFlag = true;

	return m_pData->rgnLock.LockLogic( *pLockInfo, filename, linenumber );
}

ArcadiaLock	ArcadiaServer::_LockWorld( const char * filename, int linenumber )
{
	ArBoxCollision box( true );
	return ArcadiaLock( arcadia_lock( &box, filename, linenumber ) );
}

ArcadiaLock ArcadiaServer::_LockWithVisibleRange( const char * filename, int linenumber, unsigned rx, unsigned ry, short layer )
{
	ArBoxCollision box( rx, ry, layer );
	return ArcadiaLock( arcadia_lock( &box, filename, linenumber ) );
}

ArcadiaLock ArcadiaServer::_LockArea( const char * filename, int linenumber, unsigned rx1, unsigned ry1, unsigned rx2, unsigned ry2, short layer )
{
	ArBoxCollision box( rx1, ry1, rx2, ry2, layer );
	return ArcadiaLock( arcadia_lock( &box, filename, linenumber ) );
}

ArcadiaLock	ArcadiaServer::_LockObjectWithSpecificRange( const char * filename, int linenumber, const ArObject *pObject, unsigned range )
{
	unsigned rx = pObject->GetRX();
	unsigned ry = pObject->GetRY();
	short layer = pObject->GetLayer();
	ArBoxCollision box( rx, ry, range, layer );

	ArcadiaLock _lock = ArcadiaLock( arcadia_lock( &box, filename, linenumber ) );

	while( rx != pObject->GetRX() || ry != pObject->GetRY() )
	{
		UnLock( &_lock );

		rx = pObject->GetRX();
		ry = pObject->GetRY();
		layer = pObject->GetLayer();
		ArBoxCollision new_box( rx, ry, range, layer );

		_lock = ArcadiaLock( arcadia_lock( &new_box, filename, linenumber ) );
	}

	return _lock;
}

ArcadiaLock	ArcadiaServer::_LockObjectWithVisibleRange( const char * filename, int linenumber, const ArObject *pObject )
{
	unsigned rx = pObject->GetRX();
	unsigned ry = pObject->GetRY();
	short layer = pObject->GetLayer();
	ArBoxCollision box( rx, ry, layer );

	ArcadiaLock _lock = ArcadiaLock( arcadia_lock( &box, filename, linenumber ) );

	while( rx != pObject->GetRX() || ry != pObject->GetRY() || layer != pObject->GetLayer() )
	{
		UnLock( &_lock );

		rx = pObject->GetRX();
		ry = pObject->GetRY();
		layer = pObject->GetLayer();
		ArBoxCollision new_box( rx, ry, layer );

		_lock = ArcadiaLock( arcadia_lock( &new_box, filename, linenumber ) );
	}

	return _lock;
}

ArcadiaLock	ArcadiaServer::_LockObjectWithSpecificRegion( const char * filename, int linenumber, const ArObject * pObject, unsigned rx, unsigned ry, short layer )
{
	// 다른 Lock 계열 함수는 layer 인자가 생략돼서 기본값(-1)이 들어오면 전체 레이어에 락을 걸지만 이 함수는 기본값이 -2로 다르며,
	// 기본값으로 들어오면(인자가 입력되지 않았으면) pObject의 layer에만 락을 거는 것으로, -1로 인자가 명시적으로 들어오면 전체 레이어에 락을 거는 것으로 처리됨.

	unsigned rx2 = pObject->GetRX();
	unsigned ry2 = pObject->GetRY();
	short prev_layer = pObject->GetLayer();
	// 두 개의 서로 다른 레이어에 대해 락을 거는 것은 불가능하므로 pObject와 지정 Region이 서로 다른 레이어에 있다면 전체 레이어에 대해 락을 걸어야 함(레이어 번호 -1)
	short lock_layer = ( layer == -2 ) ? prev_layer : ( ( prev_layer == layer ) ? layer : -1 );
	ArBoxCollision box( rx, ry, rx2, ry2, lock_layer );

	ArcadiaLock _lock = ArcadiaLock( arcadia_lock( &box, filename, linenumber ) );

	while( rx2 != pObject->GetRX() || ry2 != pObject->GetRY() || prev_layer != pObject->GetLayer() )
	{
		UnLock( &_lock );

		rx2 = pObject->GetRX();
		ry2 = pObject->GetRY();
		prev_layer = pObject->GetLayer();
		lock_layer = ( layer == -2 ) ? prev_layer : ( ( prev_layer == layer ) ? layer : -1 );
		ArBoxCollision new_box( rx, ry, rx2, ry2, lock_layer );

		_lock = ArcadiaLock( arcadia_lock( &new_box, filename, linenumber ) );
	}

	return _lock;
}

ArcadiaLock	ArcadiaServer::_LockObject( const char * filename, int linenumber, const ArObject *pObject )
{
	unsigned rx = pObject->GetRX();
	unsigned ry = pObject->GetRY();
	short layer = pObject->GetLayer();
	ArBoxCollision box( rx, ry, static_cast< unsigned >( -1 ), layer );

	ArcadiaLock _lock = ArcadiaLock( arcadia_lock( &box, filename, linenumber ) );

	while( rx != pObject->GetRX() || ry != pObject->GetRY() || layer != pObject->GetLayer() )
	{
		UnLock( &_lock );

		rx = pObject->GetRX();
		ry = pObject->GetRY();
		layer = pObject->GetLayer();
		ArBoxCollision new_box( rx, ry, static_cast< unsigned >( -1 ), layer );

		_lock = ArcadiaLock( arcadia_lock( &new_box, filename, linenumber ) );
	}

	return _lock;
}

ArcadiaLock ArcadiaServer::_LockObjects( const char * filename, int linenumber, const ArObject * pObject1, const ArObject * pObject2 )
{
	unsigned rx1 = pObject1->GetRX();
	unsigned ry1 = pObject1->GetRY();
	short layer1 = pObject1->GetLayer();

	unsigned rx2 = pObject2->GetRX();
	unsigned ry2 = pObject2->GetRY();
	short layer2 = pObject2->GetLayer();

	// 두 개의 서로 다른 레이어에 락을 같이 거는 것은 불가능하므로 두 Object가 서로 다른 레이어에 있다면 전체 레이어에 대해 락을 걸어야 함(레이어 번호 -1)
	short lock_layer = ( layer1 == layer2 ) ? layer1 : -1;
	ArBoxCollision box( rx1, ry1, rx2, ry2, lock_layer );

	ArcadiaLock _lock = ArcadiaLock( arcadia_lock( &box, filename, linenumber ) );

	while( true )
	{
		if( rx1 != pObject1->GetRX() || ry1 != pObject1->GetRY() || layer1 != pObject1->GetLayer() )
		{
			UnLock( &_lock );

			rx1 = pObject1->GetRX();
			ry1 = pObject1->GetRY();
			layer1 = pObject1->GetLayer();

			lock_layer = ( layer1 == layer2 ) ? layer1 : -1;
		}
		else if( rx2 != pObject2->GetRX() || ry2 != pObject2->GetRY() || layer2 != pObject2->GetLayer() )
		{
			UnLock( &_lock );

			rx2 = pObject2->GetRX();
			ry2 = pObject2->GetRY();
			layer2 = pObject2->GetLayer();

			lock_layer = ( layer1 == layer2 ) ? layer1 : -1;
		}
		else
		{
			// 두 Object가 모두 락 안에 잡혔음
			break;
		}

		ArBoxCollision new_box( rx1, ry1, rx2, ry2, lock_layer );
		_lock = ArcadiaLock( arcadia_lock( &new_box, filename, linenumber ) );
	}

	return _lock;
}

void ArcadiaServer::UnLock( ArcadiaLock * pLockHandle )
{
	if( pLockHandle->handle < 0 ) return;

	m_pData->rgnLock.UnLock( pLockHandle->handle );

	s_LockFlag = false;
}

const bool ArcadiaServer::IsLocked( const unsigned int rx, const unsigned int ry, const short layer, const bool bPartial )
{
	return m_pData->rgnLock.IsLocked( ArBoxCollision( rx, ry, layer ), bPartial );
}

const bool ArcadiaServer::IsLocked( unsigned rx1, unsigned ry1, unsigned rx2, unsigned ry2, const short layer, const bool bPartial )
{
	return m_pData->rgnLock.IsLocked( ArBoxCollision( rx1, ry1, rx2, ry2, layer ), bPartial );
}

const bool ArcadiaServer::IsLocked( const ArObject *pObject, const bool bPartial )
{
	return IsLocked( pObject->GetRX(), pObject->GetRY(), pObject->GetLayer(), bPartial );
}

unsigned ArcadiaServer::BroadcastToSpecificRegion( unsigned rx, unsigned ry, unsigned range, unsigned char layer, const void * msg )
{
	struct _MessageSender : ArObjectFunctor
	{
		_MessageSender( const void *_msg, ArcadiaIntf *_pIntf ) { msg = _msg; pIntf = _pIntf; }

		void operator()( ArObject *pObj ) const
		{
			pIntf->SendGameMessage( pObj, msg );
		}

		ArcadiaIntf * pIntf;
		const void *msg;

	} _sender( msg, m_pIntf );

	struct Sender : ArRegionFunctor
	{
		Sender( ArObjectFunctor *_pFo ) { pFo = _pFo; }

		void operator()( const struct ArRegion * pRegion )
		{
			pRegion->DoEachClient( *pFo );
		}

		ArObjectFunctor * pFo;

	} _fo( &_sender );

	m_pData->pRgnMgr->DoEachSpecificRegion( rx, ry, range, layer, _fo );

	return 0;
}

unsigned ArcadiaServer::Broadcast( unsigned rx, unsigned ry, unsigned char layer, const void * msg )
{
	struct _MessageSender : ArObjectFunctor
	{
		_MessageSender( const void *_msg, ArcadiaIntf *_pIntf ) { msg = _msg; pIntf = _pIntf; }

		void operator()( ArObject *pObj ) const
		{
			pIntf->SendGameMessage( pObj, msg );
		}

		ArcadiaIntf * pIntf;
		const void *msg;

	} _sender( msg, m_pIntf );

	struct Sender : ArRegionFunctor
	{
		Sender( ArObjectFunctor *_pFo ) { pFo = _pFo; }

		void operator()( const struct ArRegion * pRegion )
		{
			pRegion->DoEachClient( *pFo );
		}

		ArObjectFunctor * pFo;

	} _fo( &_sender );

	m_pData->pRgnMgr->DoEachVisibleRegion( rx, ry, layer, _fo );

	return 0;
}

unsigned ArcadiaServer::Broadcast( unsigned rx1, unsigned ry1, unsigned rx2, unsigned ry2, unsigned char layer, const void * msg )
{
	struct _MessageSender : ArObjectFunctor
	{
		_MessageSender( const void *_msg, ArcadiaIntf *_pIntf ) { msg = _msg; pIntf = _pIntf; }

		void operator()( ArObject *pObj ) const
		{
			pIntf->SendGameMessage( pObj, msg );
		}

		ArcadiaIntf * pIntf;
		const void *msg;

	} _sender( msg, m_pIntf );

	struct Sender : ArRegionFunctor
	{
		Sender( ArObjectFunctor *_pFo ) { pFo = _pFo; }

		void operator()( const struct ArRegion * pRegion )
		{
			pRegion->DoEachClient( *pFo );
		}

		ArObjectFunctor * pFo;

	} _fo( &_sender );

	m_pData->pRgnMgr->DoEachVisibleRegion( rx1, ry1, rx2, ry2, layer, _fo );

	return 0;
}

void ArcadiaServer::AddObject( ArObject* pObject )
{
	assert( !pObject->IsInWorld() );

	ArRegion *pRegion = m_pData->pRgnMgr->GetRegion( pObject );
	
	struct _ArRegionFunctor : ArRegionFunctor
	{
		// 서로에게 ENTER 메세지를 보낼때
		struct _SendEnterMessageEachOther : ArObjectFunctor
		{
			_SendEnterMessageEachOther( ArObject * _pWho )	{ pWho = _pWho; }

			void operator()( ArObject * pClient ) const
			{
				if( pClient == pWho ) return;

				if( pClient->GetObjectType() == ArObject::CLIENT_OBJECT ) sendEnterMessage( pClient, pWho, true );
				if( pWho->GetObjectType() == ArObject::CLIENT_OBJECT ) sendEnterMessage( pWho, pClient, true );
			}

			ArObject * pWho;
		};

		// 일방적으로 ENTER 메세지를 보낼때
		struct _SendEnterMessage : ArObjectFunctor
		{
			_SendEnterMessage( const ArObject * _pClient )	{ pClient = _pClient; }

			void operator()( ArObject * pObject ) const
			{
				sendEnterMessage( pClient, pObject, true );
			}

			const ArObject * pClient;
		};

		_ArRegionFunctor( ArObject * _pObject ) { pObject = _pObject; }

		void operator()( const ArRegion * pRegion )
		{
			_SendEnterMessageEachOther enter_msg_func( pObject );
			pRegion->DoEachClient( enter_msg_func );

			if( pObject->GetObjectType() == ArObject::CLIENT_OBJECT )
			{
				_SendEnterMessage enter( pObject );
				pRegion->DoEachStaticObject( enter );
				pRegion->DoEachMovableObject( enter );
			}
		}

		ArObject *pObject;
	} _rf( pObject );

	// 근처에 메세지 전송
	m_pData->pRgnMgr->DoEachVisibleRegion( pObject->GetRX(), pObject->GetRY(), pObject->GetLayer(), _rf );

	switch( pObject->GetObjectType() )
	{
	case ArObject::STATIC_OBJECT:	InterlockedIncrement( &m_nItemCount ); return pRegion->AddStaticObject( pObject );
	case ArObject::MOVABLE_OBJECT:	InterlockedIncrement( &m_nNPCCount );  return pRegion->AddMovableObject( pObject );
	case ArObject::CLIENT_OBJECT:	InterlockedIncrement( &m_nUserCount ); return pRegion->AddClientObject( pObject );
	}
}

void ArcadiaServer::RemoveObject( ArObject* pObject )
{
	ArRegion *pRegion = m_pData->pRgnMgr->GetRegion( pObject );

	if( g_bUseRegionDebug && !IsLocked( pObject->GetRX(), pObject->GetRY(), pObject->GetRX(), pObject->GetRY(), pObject->GetLayer() ) )
	{
		assert( 0 );
		XSEH::InvokeUnhandledException( STATUS_ILLEGAL_INSTRUCTION );
	}

	switch( pObject->GetObjectType() )
	{
	case ArObject::STATIC_OBJECT:	InterlockedDecrement( &m_nItemCount ); 	pRegion->RemoveStaticObject( pObject ); break;
	case ArObject::MOVABLE_OBJECT:	InterlockedDecrement( &m_nNPCCount );  	pRegion->RemoveMovableObject( pObject ); break;
	case ArObject::CLIENT_OBJECT:	InterlockedDecrement( &m_nUserCount ); 	pRegion->RemoveClientObject( pObject ); break;
	}
	
	struct _ArRegionFunctor : ArRegionFunctor
	{
		// Leave 메세지를 보냄
		struct _SendLeaveMessage : ArObjectFunctor
		{
			_SendLeaveMessage( const ArObject * _pWho )
			{
				pWho = _pWho;
			}

			virtual void operator()( ArObject * pClient ) const
			{
				if( pClient->GetObjectType() == ArObject::CLIENT_OBJECT )	ArcadiaServer::GetIntf()->SendLeaveMessage( pClient, pWho );
			}

			const ArObject * pWho;
		};

		_ArRegionFunctor( const ArObject * _pObject ) { pObject = _pObject; }

		void operator()( const ArRegion * pRegion )
		{
			_SendLeaveMessage func( pObject );
			pRegion->DoEachClient( func );
		}

		const ArObject *pObject;
	} _rf( pObject );

	// 근처에 메세지 전송
	m_pData->pRgnMgr->DoEachVisibleRegion( pObject->GetRX(), pObject->GetRY(), pObject->GetLayer(), _rf );
}

void ArcadiaServer::step( ArObject * pObject, AR_TIME tm )
{
	const ArPosition oldPos = pObject->GetPos();
	pObject->Step( tm );
	const ArPosition newPos = pObject->GetPos();

	onMoveObject( pObject, oldPos, newPos );
	
	pObject->lastStepTime = tm;
}

void ArcadiaServer::onMoveObject( ArObject * pObject, const ArPosition & oldPos, const ArPosition & newPos )
{
	unsigned prev_rx = oldPos.GetRX();
	unsigned prev_ry = oldPos.GetRY();

	unsigned rx = newPos.GetRX();
	unsigned ry = newPos.GetRY();

	short layer = pObject->GetLayer();

	if( prev_rx != rx || prev_ry != ry )
	{
		if( g_bUseRegionDebug && !IsLocked( prev_rx, prev_ry, rx, ry, layer ) )
		{
			assert( 0 );
			XSEH::InvokeUnhandledException( STATUS_ILLEGAL_INSTRUCTION );
		}

		m_pData->pRgnMgr->GetRegion( prev_rx, prev_ry, pObject->GetLayer() )->RemoveObject( pObject );
		m_pData->pRgnMgr->GetRegion( pObject )->AddObject( pObject );
	}
}

void ArcadiaServer::enterProc( ArObject * pObject, unsigned prx, unsigned pry )
{
	if( pObject->GetRX() == prx && pObject->GetRY() == pry ) return;

	// 서로 enter 처리
	struct _ArRegionFunctor : ArRegionFunctor
	{
		_ArRegionFunctor()
		{
			bIsSent = false;
			pObject = NULL;
			bIsClientObject = false;
		}

		void operator()( const struct ArRegion * pRegion )
		{
			if( !pRegion ) return;

			// 서로에게 ENTER 메세지를 보낼때
			struct _SendEnterMessageEachOther : ArObjectFunctor
			{
				_SendEnterMessageEachOther( ArObject * _pWho )	{ pWho = _pWho; }

				virtual void operator()( ArObject * pClient ) const
				{
					if( pClient == pWho ) return;
					sendEnterMessage( pClient, pWho );
					if( pWho->GetObjectType() == ArObject::CLIENT_OBJECT ) sendEnterMessage( pWho, pClient );
				}

				ArObject * pWho;
			};

			// 일방적인 ENTER 메세지를 보낼때
			struct _SendEnterMessage : ArObjectFunctor
			{
				_SendEnterMessage( ArObject * _pClient )	{ pClient = _pClient; }

				virtual void operator()( ArObject * pObject ) const
				{
					sendEnterMessage( pClient, pObject );

					// pClient 에게 ENTER 메세지를 
					if( pObject->GetObjectType() == ArObject::MOVABLE_OBJECT )
					{
						pObject->bIsNearClient = true;
	
						if( pObject->GetFinalPriority() < ArObject::UPDATE_PRIORITY_NORMAL )
							ArcadiaServer::Instance().SetObjectPriority( pObject, ArObject::UPDATE_PRIORITY_NORMAL );
					}
				}

				ArObject * pClient;
			};

			_SendEnterMessageEachOther enter_each( pObject );
			if( pRegion->DoEachClient( enter_each ) ) bIsSent = true;

			if( pObject->GetObjectType() == ArObject::CLIENT_OBJECT )
			{
				_SendEnterMessage enter( pObject );
				pRegion->DoEachStaticObject( enter );
				pRegion->DoEachMovableObject( enter );
			}
		}
		
		ArObject *pObject;
		bool bIsClientObject;
		bool bIsSent;
	};

	// { enter 메세지 처리를 위한 함수객체 생성
	_ArRegionFunctor fo;
	fo.pObject = pObject;
	fo.bIsClientObject = ( pObject->GetObjectType() == ArObject::CLIENT_OBJECT ? true : false );
	// }

	// 새로 진입한 영역에 enter 메세지를 보낸다.
	m_pData->pRgnMgr->DoEachNewRegion( pObject->GetRX(), pObject->GetRY(), prx, pry, pObject->GetLayer(), fo );

	// 자신이 한번이라도 enter 메세지를 보낸적이 있다면 근처에 클라이언트가 있는 것이므로 활성화 시킨다.
	if( fo.bIsSent )
	{
		pObject->bIsNearClient = true;
		if( pObject->GetObjectType() == ArObject::MOVABLE_OBJECT && pObject->GetFinalPriority() < ArObject::UPDATE_PRIORITY_NORMAL )
		{
			SetObjectPriority( pObject, ArObject::UPDATE_PRIORITY_NORMAL );
		}
	}

	// region 변경이 완료되었음을 통지한다.
	if( pObject->GetObjectType() == ArObject::CLIENT_OBJECT ) m_pIntf->SendRegionAckMessage( pObject, pObject->GetRX(), pObject->GetRY() );
}

void ArcadiaServer::DeleteObject( ArSchedulerObject* pObject )
{
	assert( !pObject->IsDeleteRequested() );	

	if( pObject->GetPriority() != ArObject::UPDATE_PRIORITY_IDLE )
	{
		SetObjectPriority( pObject, ArObject::UPDATE_PRIORITY_IDLE );
	}
	
	m_pData->scheduler.DeleteObject( pObject );
}

void ArcadiaServer::EnumMovableObject( const ArPosition & pos, unsigned char layer, AR_UNIT range, std::vector< AR_HANDLE > * pvResult, bool bIncludeClient, bool bIncludeNPC )
{
	struct _ArRegionFunctor : ArRegionFunctor
	{
		struct _ArObjectFunctor : ArObjectFunctor
		{
			_ArObjectFunctor( _ArRegionFunctor * _pParent )	{ pParent = _pParent; }

			void operator()( ArObject * pObj ) const
			{
				const ArPosition c_pos = pObj->GetCurrentPosition( pParent->t );

				if( pParent->left > c_pos.x ) return;
				if( pParent->right < c_pos.x ) return;
				if( pParent->top > c_pos.y ) return;
				if( pParent->bottom < c_pos.y ) return;

				AR_UNIT dist = c_pos.GetDistance( pParent->pos );

				if( dist < pParent->range ) pParent->pvResult->push_back( pObj->GetHandle() );
			}

			_ArRegionFunctor * pParent;
		};

		_ArRegionFunctor( std::vector< AR_HANDLE > * _pvResult, const ArPosition & _pos, AR_UNIT _range, bool _bIncludeClient, bool _bIncludeNPC ) : pos( _pos ), bIncludeClient( _bIncludeClient ), bIncludeNPC( _bIncludeNPC )
		{
			range = _range;
			left = _pos.x - _range;
			right = _pos.x + _range;
			top = _pos.y - _range;
			bottom = _pos.y + _range;			
			pvResult = _pvResult;
			t = GetArTime();
		}

		void operator()( const ArRegion * pRegion )
		{
			_ArObjectFunctor func( this );
			if( bIncludeClient ) pRegion->DoEachClient( func );
			if( bIncludeNPC )	 pRegion->DoEachMovableObject( func );
		}

		std::vector< AR_HANDLE > *pvResult;

		AR_TIME t;
		const ArPosition & pos;
		AR_UNIT left, right, top, bottom;
		AR_UNIT range;
		bool bIncludeClient, bIncludeNPC;

	private:

		const _ArRegionFunctor& operator=( const _ArRegionFunctor& ) { return *this; }

	} _rf( pvResult, pos, range, bIncludeClient, bIncludeNPC );

	m_pData->pRgnMgr->DoEachVisibleRegion( pos.GetRX(), pos.GetRY(), layer, _rf );
}
void ArcadiaServer::EnumMovableObjectInEveryRegion( int rx1, int ry1, int rx2, int ry2, unsigned char layer, std::vector< AR_HANDLE > * pvResult, bool bIncludeClient, bool bIncludeNPC )
{
	struct _ArRegionFunctor : ArRegionFunctor
	{
		struct _ArObjectFunctor : ArObjectFunctor
		{
			_ArObjectFunctor( _ArRegionFunctor * _pParent )	{ pParent = _pParent; }

			void operator()( ArObject * pObj ) const
			{
				pParent->pvResult->push_back( pObj->GetHandle() );
			}

			_ArRegionFunctor * pParent;
		};

		_ArRegionFunctor( std::vector< AR_HANDLE > * _pvResult, bool _bIncludeClient, bool _bIncludeNPC ) : bIncludeClient( _bIncludeClient ), bIncludeNPC( _bIncludeNPC )
		{
			pvResult = _pvResult;
			t = GetArTime();
		}

		void operator()( const ArRegion * pRegion )
		{
			_ArObjectFunctor func( this );
			if( bIncludeClient ) pRegion->DoEachClient( func );
			if( bIncludeNPC )	 pRegion->DoEachMovableObject( func );
		}

		std::vector< AR_HANDLE > *pvResult;

		AR_TIME t;
		bool bIncludeClient, bIncludeNPC;

	} _rf( pvResult,  bIncludeClient, bIncludeNPC );

	m_pData->pRgnMgr->DoEachRegion( rx1, ry1, rx2, ry2, layer, _rf );
}

void ArcadiaServer::EnumMovableObject( int rx1, int ry1, int rx2, int ry2, unsigned char layer, std::vector< AR_HANDLE > * pvResult, bool bIncludeClient, bool bIncludeNPC )
{
	struct _ArRegionFunctor : ArRegionFunctor
	{
		struct _ArObjectFunctor : ArObjectFunctor
		{
			_ArObjectFunctor( _ArRegionFunctor * _pParent )	{ pParent = _pParent; }

			void operator()( ArObject * pObj ) const
			{
				pParent->pvResult->push_back( pObj->GetHandle() );
			}

			_ArRegionFunctor * pParent;
		};

		_ArRegionFunctor( std::vector< AR_HANDLE > * _pvResult, bool _bIncludeClient, bool _bIncludeNPC ) : bIncludeClient( _bIncludeClient ), bIncludeNPC( _bIncludeNPC )
		{
			pvResult = _pvResult;
			t = GetArTime();
		}

		void operator()( const ArRegion * pRegion )
		{
			_ArObjectFunctor func( this );
			if( bIncludeClient ) pRegion->DoEachClient( func );
			if( bIncludeNPC )	 pRegion->DoEachMovableObject( func );
		}

		std::vector< AR_HANDLE > *pvResult;

		AR_TIME t;
		bool bIncludeClient, bIncludeNPC;

	} _rf( pvResult,  bIncludeClient, bIncludeNPC );

	m_pData->pRgnMgr->DoEachVisibleRegion( rx1, ry1, rx2, ry2, layer, _rf );
}

void ArcadiaServer::DoEachVisibleRegion( int rx, int ry, unsigned char layer, ArRegionFunctor & _fo )
{	
	m_pData->pRgnMgr->DoEachVisibleRegion( rx, ry, layer, _fo );
}

void ArcadiaServer::DoEachRegion( int rx1, int ry1, int rx2, int ry2, unsigned char layer, ArRegionFunctor & _fo )
{
	m_pData->pRgnMgr->DoEachRegion( rx1, ry1, rx2, ry2, layer, _fo );
}

void ArcadiaServer::DoEachVisibleRegion( int rx1, int ry1, int rx2, int ry2, unsigned char layer, ArRegionFunctor & _fo )
{	
	m_pData->pRgnMgr->DoEachVisibleRegion( rx1, ry1, rx2, ry2, layer, _fo );
}
