#ifndef LOCALE
#define LOCALE Unknown
#endif

#define STR(x) #x
#define XSTR(x) STR(x)

#include <cstdio>
#include <vector>

#include <network/XIOCP.h>
#include <network/XIOCPAcceptor.h>
#include <network/XNetworkUtil.h>
#include <mmo/ArcadiaServer.h>
#include <toolkit/TimeSyncer.h>
#include <toolkit/XConsole.h>
#include <toolkit/myDC.h>
#include <toolkit/XEnv.h>
#include <toolkit/XStringUtil.h>
#include <logging/FileLog.h>

#include "LogClient/LogClient.h"

#include "GameNetwork.h"
#include "SendMessage.h"

#include "StructItem.h"
#include "StructMonster.h"
#include "StructPlayer.h"
#include "StructSummon.h"
#include "StructNPC.h"
#include "StructSkill.h"
#include "GameMessage.h"
#include "GameAllocator.h"
#include "Extern.h"
#include "ItemCollector.h"
#include "Constant.h"
#include "PartyManager.h"

#include "DB_Commands.h"

#include "GameProc.h"
#include "XSecuritySolutionManager.h"
#include "AgeLimitRule.h"
#include "framework/ArcadiaFramework.h"

extern XIOCP			*				g_pIOCP;
extern ArcadiaServer	*				g_pArcadia;

IStreamSocketConnection	*				g_pAuthConnection;
IStreamSocketConnection	*				g_pUploadConnection;
IStreamSocketConnection *				g_pCoordinatorConnection;

volatile LONG							g_nRecvMessageCount;
volatile LONG							g_nSendMessageCount;
volatile LONG							g_nRecvMessageByte;
volatile LONG							g_nSendMessageByte;
extern volatile LONG					g_nConnectionCount;

bool									g_bUseMessageStatistics;

extern XCriticalSection					g_ConnectionTagLock;

extern __declspec( thread ) XSEH::THREAD_INFO	s_ThreadInfo;

ConnectionManager	g_ConnectionManager;
ConnectionCloser	g_ConnectionCloser;
XIOCPAcceptor *		g_pMyAcceptor = NULL;

IConnection* myNetworkEventReceiver::createConnection( const XSocket & socket )
{
	return new XIOCPConnection( g_pIOCP->getOverlappedAllocator(), socket, true );
}

bool myNetworkEventReceiver::onAccept( int nID, IAcceptor * pAcceptor, IConnection * pConnection )
{
	std::string strIPMask = ENV().GetString( "io.ip_mask", "" );
	if( strIPMask.size() )
	{
		std::vector< std::string > vToken;
		XStringUtil::Split( strIPMask.c_str(), vToken, ";", false  );

		std::vector< std::string >::iterator it;
		for( it = vToken.begin(); it != vToken.end(); ++it )
		{
			if( strstr( pConnection->GetPeerAddress().GetAddr(), (*it).c_str() ) ) break;
		}

		if( it == vToken.end() ) 
		{
			delete pConnection;
			return false;
		}
	}

	{
		THREAD_SYNCRONIZE( g_ConnectionTagLock );
	
		pConnection->SetTag( new _CONNECTION_TAG( "" ) );
		static_cast< IStreamSocketConnection * >( pConnection )->EnableNoDelay();
	}
	g_ConnectionManager.Push( pConnection );

	return true;
}

void myNetworkEventReceiver::onRead( int nID, IConnection * pConnection )
{
	void onReadEvent( int nID, IStreamSocketConnection * pConn );
	onReadEvent( nID, (IStreamSocketConnection *)pConnection );
}

void ConnectionManager::onProcess( int nThreadIdx )
{
	char buf[255];
	s_sprintf( buf, _countof( buf ), "thread.scheduler.%d.proc", nThreadIdx );
	ENV().Set( buf, "ConnectionManager" );

	s_sprintf( s_ThreadInfo.job_info, _countof( s_ThreadInfo.job_info ), "ConnectionManager(0x%08X)", (UINT_PTR)this );
	s_ThreadInfo.last_execute_time = GetArTime();

	std::vector< IConnection * > vClosedConnection;
	std::vector< IConnection * >::iterator itClosedCon;

	{		
		THREAD_SYNCRONIZE( m_ConnectionCS );

		std::list< IConnection* >::iterator itCon;

		InitAgeLimitPerod();

		for( itCon = m_vConnection.begin(); itCon != m_vConnection.end(); ++itCon )
		{
			THREAD_SYNCRONIZE( g_ConnectionTagLock );

			AR_TIME currentTime = GetArTime(); // 앞에서 미리 받아놓고 계산하니까 삐꾸나는 것 같아서 그때그때 받기로 했음.
			
#ifndef _DEBUG
			_CONNECTION_TAG * pTag = reinterpret_cast< _CONNECTION_TAG * > ( (*itCon)->GetTag() );
			if( (*itCon)->IsConnected() && currentTime - pTag->nLastReadTime > 60000 )
			{
				if( !(pTag->pPlayer) || pTag->pPlayer->GetPermission() < GameRule::PERMISSION_FOR_GM )
				{
					vClosedConnection.push_back( (*itCon) );
					continue;
				}
			}
#endif


			if( CheckAgeLimitPerod( static_cast< IStreamSocketConnection * >( (*itCon) ) ) )
			{
				vClosedConnection.push_back( (*itCon) );
				continue;
			}

			if( GameRule::nSecuritySolutionType != XSecuritySolutionManager::TYPE_NONE )
			{
				if( !XSecuritySolutionManager::Instance().ProcValidation( static_cast< IStreamSocketConnection * >( (*itCon) ) ) )
					vClosedConnection.push_back( (*itCon) );
			}
		}

		for( itClosedCon = vClosedConnection.begin(); itClosedCon != vClosedConnection.end(); ++itClosedCon )
		{
			assert( (*itClosedCon) != NULL );
			THREAD_SYNCRONIZE( g_ConnectionTagLock );

			_CONNECTION_TAG * pTag = reinterpret_cast< _CONNECTION_TAG * > ( (*itClosedCon)->GetTag() );
			int nElapsedTimeAfterLastPacket = ( GetArTime() - pTag->nLastReadTime ) / 100;
			const char * pszAccountName = ( strlen( pTag->szAccountName ) ) ? pTag->szAccountName : "No Account";

			XIOCPConnection *pConnection = static_cast< XIOCPConnection * >( *itClosedCon );
			int nPrevPendingQueryCount = pConnection->GetPendingQueryCount();
			int nPrevPendingRecvQueryCount = pConnection->GetPendingRecvQueryCount();
			int nPrevPendingSendQueryCount = pConnection->GetPendingSendQueryCount();
			int nPrevVar = pConnection->GetVar();

			if( (*itClosedCon)->IsConnected() )
				(*itClosedCon)->Close();

			FILELOG( "Closing connection. Last packet received %d seconds ago from %s[%s]. ConnectionInfo[%d(%d/%d),%d][%d(%d/%d),%d]",
				nElapsedTimeAfterLastPacket, (*itClosedCon)->GetPeerAddress().GetAddr(), pszAccountName,
				nPrevPendingQueryCount, nPrevPendingRecvQueryCount, nPrevPendingSendQueryCount, nPrevVar,
				pConnection->GetPendingQueryCount(), pConnection->GetPendingRecvQueryCount(), pConnection->GetPendingSendQueryCount(), pConnection->GetVar() );
			_cprint( "Closing connection. Last packet received %d seconds ago from %s[%s]. ConnectionInfo[%d(%d/%d),%d][%d(%d/%d),%d]\n",
				nElapsedTimeAfterLastPacket, (*itClosedCon)->GetPeerAddress().GetAddr(), pszAccountName,
				nPrevPendingQueryCount, nPrevPendingRecvQueryCount, nPrevPendingSendQueryCount, nPrevVar,
				pConnection->GetPendingQueryCount(), pConnection->GetPendingRecvQueryCount(), pConnection->GetPendingSendQueryCount(), pConnection->GetVar() );
		}
	}
}

void ConnectionManager::DisconnectAll()
{
	std::list< IConnection * > vConnection;

	// m_ConnectionCS 걸고 XIOCPConnection::Close 호출하면 락 관련 문제 발생하여 사본 리스트 만들고 사본을 사용함
	{
		THREAD_SYNCRONIZE( m_ConnectionCS );
		vConnection = m_vConnection;
	}

	std::list< IConnection * >::iterator itCon;
	for( itCon = vConnection.begin(); itCon != vConnection.end(); ++itCon )
	{
		if( (*itCon)->IsConnected() )
			(*itCon)->Close();
	}
}

void ConnectionCloser::onProcess( int nThreadIdx )
{
	char buf[255];
	s_sprintf( buf, _countof( buf ), "thread.scheduler.%d.proc", nThreadIdx );
	ENV().Set( buf, "ConnectionCloser" );

	s_sprintf( s_ThreadInfo.job_info, _countof( s_ThreadInfo.job_info ), "ConnectionCloser(0x%08X)", (UINT_PTR)this );
	s_ThreadInfo.last_execute_time = GetArTime();

	THREAD_SYNCRONIZE( m_ClosedConnectionCS );

	std::list< XIOCPConnection* >::iterator it;
	for( it = m_vClosedConnection.begin(); it != m_vClosedConnection.end(); )
	{
		if( (*it)->GetVar() ) 
		{
			++it;
			continue;
		}

		if( (*it) != static_cast< XIOCPConnection * >( g_pAuthConnection ) && (*it) != static_cast< XIOCPConnection * >( g_pUploadConnection ) && (*it) != static_cast< XIOCPConnection * >( g_pCoordinatorConnection ) )
		{
			THREAD_SYNCRONIZE( g_ConnectionTagLock );

			_CONNECTION_TAG* pTag = static_cast< _CONNECTION_TAG* >( (*it)->GetTag() );

			if( pTag )
			{
				StructPlayer * pPlayer = pTag->pPlayer;

				if( pPlayer )
				{
					// 완전히 로그아웃 처리가 되기 전까지는 UnRegisterAccount가 안 되므로
					// GetConnectionByAccount가 delete된 pConnection을 반환하지 않도록 NULL로 재설정해야 함
					// UnRegisterAccount를 사용할 경우 유저의 로그인이 가능해지므로 등록 상태이되 값만 NULL로 바꿈
					StructPlayer::SetRegisteredConnectionByAccount( pTag->szAccountName, NULL );

					// pConnection을 수정하는 것이 LogoutNow와 중첩되지 않도록 하기 위해 지역 락 사용
					{
						ARCADIA_LOCK( ArcadiaServer::Instance().LockObjectWithVisibleRange( pPlayer ) );

						// OnConnectionClosed에서 IsInWorld(), IsLogin() 이 정확히 처리되도록 하기 위해서
						// 지역 락 걸린 상태에서 호출
						pPlayer->OnConnectionClosed( pTag );

						pPlayer->pConnection = NULL;
					}
				}
				else if( pTag->szAccountName[0] && pTag->bAuthByAuthServer )
				{
					StructPlayer::UnRegisterAccount( pTag->szAccountName );
					SendLogoutToAuth( pTag->szAccountName, pTag->nContinuousPlayTime, 4 );
					pTag->bAuthByAuthServer = false;
				}

				if( pTag->nConnId )
				{
					StructPlayer::EraseReturnLobbyConnection( pTag->nConnId );
				}

				XSecuritySolutionManager::Instance().DeinitClientSession( *it );

				delete pTag;
				(*it)->SetTag( NULL );
			}

			delete *it;
		}
		it = m_vClosedConnection.erase( it );

	}
}

void deleteConnection( struct IStreamSocketConnection* pConnection )
{
	g_ConnectionCloser.Push( static_cast< XIOCPConnection* >( pConnection ) );
}

void myNetworkEventReceiver::onDisconnect( int nID, IConnection * pConnection )
{
	try
	{
		if( pConnection != static_cast< IConnection * >( g_pAuthConnection ) && pConnection != static_cast< IConnection * >( g_pUploadConnection ) && pConnection != static_cast< IConnection * >( g_pCoordinatorConnection ) )
		{
			THREAD_SYNCRONIZE( g_ConnectionTagLock );

			_CONNECTION_TAG* pTag = static_cast< _CONNECTION_TAG* >( pConnection->GetTag() );

			// 여기서 deleteConnection을 호출하여 ConnectionCloser에게 넘겨주므로 pConnection이 유효하다면
			// pTag도 절대로 유효해야 함. 유효하지 않다면 하나의 Connection을 2번째 onDisconnect 호출하는 경우임.
			assert( pTag );

			// pTag->pPlayer 가 NULL이 아니면 계정정보에 관한 처리는 player쪽으로 넘겨라
			if( pTag && pTag->szAccountName[0] && pTag->pPlayer == NULL )
			{
				if( pTag->bAuthByAuthServer )
				{
					StructPlayer::UnRegisterAccount( pTag->szAccountName );
					SendLogoutToAuth( pTag->szAccountName, pTag->nContinuousPlayTime, 5 );
					pTag->bAuthByAuthServer = false;
				}
			}
		}
		g_ConnectionManager.Pop( pConnection );

		deleteConnection( static_cast< IStreamSocketConnection* >( pConnection ) );
	}
	catch( ... )
	{
		return;
	}
}

void myNetworkEventReceiver::onError( const char * szError )
{
	_lprint( "GameLog.txt", "EXCEPTION:%s\n", szError );
}



struct MyArcadiaIntf : ArcadiaIntf
{
	void SendEnterMessage( const ArObject * pClient, const ArObject * pObj )
	{
		SendEnterMsg( pClient, pObj );
	}

	void SendForceMoveMessage( const ArObject * pClient, const ArObject * pObj )
	{
		//SendForceMoveMsg( pClient, pObj );
	}

	void SendMoveMessage( const ArObject * pClient, const ArObject * pObj )
	{
		SendMoveMsg( pClient, pObj );
	}

	void SendMoveAckMessage( const ArObject * pClient, AR_TIME tm, unsigned char speed )
	{
		TS_MOVE_ACK msg;
		msg.time = tm;
		msg.speed = speed;
		PendMessage( pClient, &msg );

#ifdef MOVE_DEBUG
		_cprint( "SND : TM_SC_MOVE_ACK\n" );
#endif
	}

	void SendLeaveMessage( const ArObject * pClient, const ArObject * pObj )
	{
		SendLeaveMsg( pClient, pObj );

#ifdef MOVE_DEBUG
		_cprint( "SND : TM_SC_LEAVE (%d)\n", msg.handle );
#endif
	}

	void SendRegionAckMessage( const ArObject * pClient, unsigned rx, unsigned ry )
	{
		TS_REGION_ACK msg;
		msg.rx = rx;
		msg.ry = ry;

		PendMessage( pClient, &msg );
	}

	void SendGameMessage( const ArObject * pClient, const void *Message )
	{
		PendMessage( pClient, (TS_MESSAGE*)Message );
	}

	bool onSetMove( ArObject * pObj, const ArPosition & oldPos, const ArPosition & newPos )
	{
		AR_HANDLE handle = pObj->GetHandle();
		GameObject *pPtr = GameObject::raw_get( handle );
		
		if( pPtr && (	IsPlayer( handle ) ||
						IsMonster( handle ) ||
						IsSummon( handle ) ) )
		{
			StructCreature *pCreature = static_cast< StructCreature * >( pPtr );

			// 이동할 장소에 뭔가가 이미 있다면 이동실패
			if( !StructCreature::QuadTreeItem::IsCanAdd( newPos.x, newPos.y ) ) return false;
			
			// 기존 아이템 제거하고
			pCreature->quadTreeItem.RemoveMe();

			// 새 위치에 추가
			pCreature->quadTreeItem.Set( newPos.x, newPos.y );

			pCreature->quadTreeItem.AddMe();
		}

		return true;
	}

} g_ArcadiaIntf;


void iocp_thread_init_func( int nThreadNum )
{
	s_sprintf( s_ThreadInfo.thread_name, _countof( s_ThreadInfo.thread_name ), "IOCP %02d", nThreadNum );
	s_ThreadInfo.job_info[0] = '\0';
	s_ThreadInfo.last_execute_time = 0;

	XSEH::AddThreadInfo( &s_ThreadInfo );

}

void InitNetwork()
{
	static myNetworkEventReceiver	myReceiver;
	static XIOCP					myIOCP( &myReceiver );
	
	g_pIOCP = &myIOCP;

	_cprint( "IOCP initialize...   " );
	if( myIOCP.Init() )
	{
		_cprint( "ok\n" );
		FILELOG( "IOCP initialize...  ok" );
	}
	else
	{
		_cprint( "failed\n" );
		FILELOG( "IOCP initialize...  failed" );
	}
	
	_cprint( "Starting IOCP thread pool...   " );
	if( myIOCP.StartThreadPool(ENV().GetInt( "io.iocp_thread", 4 ), iocp_thread_init_func, true ) )
	{
		_cprint( "ok\n" );
		FILELOG( "Starting IOCP thread pool...  ok" );
	}
	else
	{
		_cprint( "Failed\n" );
		FILELOG( "Starting IOCP thread pool...  failed" );
	}
}

void InitServerStatusReport()
{
	struct _ServerStatusReport : public ArSchedulerObject
	{
		virtual void onProcess( int nThreadIdx )
		{
			char buf[255];
			s_sprintf( buf, _countof( buf ), "thread.scheduler.%d.proc", nThreadIdx );
			ENV().Set( buf, "_ServerStatusReport" );

			s_sprintf( s_ThreadInfo.job_info, _countof( s_ThreadInfo.job_info ), "_ServerStatusReport(0x%08X)", (UINT_PTR)this );
			s_ThreadInfo.last_execute_time = GetArTime();

			LOG::Log11N4S( LM_SERVER_STATUS, 0, 0, 0, ENV().GetInt( "game.user_count" ), ENV().GetInt( "process.load" ), ENV().GetInt( "process.memory" ), g_nRecvMessageByte, g_nSendMessageByte, 0, 0, 0, "", 0,  "", 0, "", 0, "", 0 );
		}
	};

	static _ServerStatusReport _inst;

	ArcadiaServer::Instance().SetObjectPriority( &_inst, ArSchedulerObject::UPDATE_PRIORITY_LOW );
}

void arcadia_init_func( int nThreadNum )
{
	s_sprintf( s_ThreadInfo.thread_name, _countof( s_ThreadInfo.thread_name ), "Scheduler %02d", nThreadNum );
	s_ThreadInfo.job_info[0] = '\0';
	s_ThreadInfo.last_execute_time = 0;

	XSEH::AddThreadInfo( &s_ThreadInfo );
}

void InitArcadia()
{
	g_nMapWidth = ENV().GetInt( "game.map_width", g_nMapWidth );
	g_nMapHeight = ENV().GetInt( "game.map_height", g_nMapHeight );

	ENV().Bind( "io.recv_count", (int*)&g_nRecvMessageCount );
	ENV().Bind( "io.send_count", (int*)&g_nSendMessageCount );
	ENV().Bind( "io.recv_byte", (int*)&g_nRecvMessageByte );
	ENV().Bind( "io.send_byte", (int*)&g_nSendMessageByte );
	ENV().Bind( "io.connection", (int*)&g_nConnectionCount );
	
	ENV().Bind( "io.use_message_statistics", &g_bUseMessageStatistics );

	ENV().Bind( "game.map_width",	&g_nMapWidth );
	ENV().Bind( "game.map_height",	&g_nMapHeight );
	ENV().Set(  "game.max_layer",	(int) MAX_LAYER );
	ENV().Set(  "game.speed_unit",	SPEED_UNIT );

	_cprint( "Launching ARCADIA...   " );
	g_pArcadia = &ArcadiaServer::Instance();
	if( g_pArcadia->Init( &g_ArcadiaIntf, g_nMapWidth, g_nMapHeight, arcadia_init_func, true ) )
	{
		_cprint( "ok\n" );
		FILELOG( "Launching ARCADIA...   ok" );
	}
	else
	{
		_cprint( "failed\n" );
		FILELOG( "Launching ARCADIA...   failed" );
	}

	ItemCollector::Instance().Init();

	InitServerStatusReport();
}

void StartAccept()
{
	static XIOCPAcceptor			myAcceptor;
	static PlayerDeleter			s_PlayerDeleter;

	g_pMyAcceptor = &myAcceptor;

	_cprint( "Acceptor initialize...   " );
	XAddr addr( ENV().GetString( "io.addr", "0.0.0.0" ).c_str(), ENV().GetInt( "io.port", 4514 ) );
	if( myAcceptor.StartAccept( addr ) )
	{
		_cprint( "ok(IP: %s, Port: %d)\n", addr.GetAddr(), addr.GetPort() );
		FILELOG( "Acceptor initialize...   ok(IP: %s, Port: %d)", addr.GetAddr(), addr.GetPort() );
	}
	else
	{
		_cprint( "failed(IP: %s, Port: %d)\n", addr.GetAddr(), addr.GetPort() );
		FILELOG( "Acceptor initialize...   failed(IP: %s, Port: %d)", addr.GetAddr(), addr.GetPort() );
	}

	_cprint( "Adding acceptor to IOCP...   " );
	if( g_pIOCP->AddObject( &myAcceptor ) )
	{
		_cprint( "ok\n" );
		FILELOG( "Adding acceptor to IOCP...   ok" );
	}
	else
	{
		_cprint( "Failed\n" );
		FILELOG( "Adding acceptor to IOCP...   failed" );
	}

	ArcadiaServer::Instance().SetObjectPriority( &g_ConnectionCloser, ArSchedulerObject::UPDATE_PRIORITY_NORMAL );
	ArcadiaServer::Instance().SetObjectPriority( &g_ConnectionManager, ArSchedulerObject::UPDATE_PRIORITY_NORMAL );


	ArcadiaServer::Instance().SetObjectPriority( &s_PlayerDeleter, ArSchedulerObject::UPDATE_PRIORITY_LOWEST );
}

void EndAccept()
{
	if( g_pMyAcceptor && g_pMyAcceptor->EndAccept() )
	{
		_cprint( "Acceptor deinitialized.\n" );
		FILELOG( "Acceptor deinitialized." );
	}
}

bool ConnectToAuth()
{
	static XIOCPConnection			authConnection( g_pIOCP->getOverlappedAllocator(), false );
	g_pAuthConnection = static_cast< IStreamSocketConnection * >( &authConnection );

	authConnection.SetID( XOBJ_CONNECTION );
			
	_cprint( "Connect To Auth... " );

	if( authConnection.SyncConnect( XAddr( ENV().GetString( "io.auth.ip", "127.0.0.1" ).c_str() , ENV().GetInt( "io.auth.port", 4502 ) ) ) )
	{
		_cprint( "ok\n" );	
		FILELOG( "Connect To Auth... ok" );
	}
	else
	{
		_cprint( "Failed\n" );
		FILELOG( "Connect To Auth... failed" );
		return false;
	}

	_cprint( "Adding auth connection to IOCP...   " );
	if( g_pIOCP->AddObject( &authConnection ) )
	{
		_cprint( "ok\n" );
		FILELOG( "Adding auth connection to IOCP...   ok" );
	}
	else
	{
		_cprint( "Failed\n" );
		FILELOG( "Adding auth connection to IOCP...   failed" );
		return false;
	}

	return true;
}

bool ConnectToUpload()
{
	extern IStreamSocketConnection * g_pUploadConnection;

	// 이미 연결되어 있으면 패스
	if( g_pUploadConnection && g_pUploadConnection->IsConnected() )
		return true;

	bool result = true;
	do
	{
		static XIOCPConnection			uploadConnection( g_pIOCP->getOverlappedAllocator(), false );
		g_pUploadConnection = static_cast< IStreamSocketConnection * >( &uploadConnection );

		uploadConnection.SetID( XOBJ_CONNECTION );

		_cprint( "Connect To Upload...   " );

		if( uploadConnection.SyncConnect( XAddr( ENV().GetString( "io.upload.ip", "127.0.0.1" ).c_str(), ENV().GetInt( "io.upload.port", 4517 ) ) ) )
		{
			_cprint( "ok\n" );
			FILELOG( "Connect To Upload...   ok" );
		}
		else
		{
			_cprint( "failed\n" );
			FILELOG( "Connect To Upload...   failed" );
			result = false;
			break;
		}

		_cprint( "Adding upload connection to IOCP...   " );
		if( g_pIOCP->AddObject( &uploadConnection ) )
		{
			_cprint( "ok\n" );
			FILELOG( "Adding upload connection to IOCP...   ok" );
		}
		else
		{
			_cprint( "failed\n" );
			FILELOG( "Adding upload connection to IOCP...   failed" );
			result = false;
			break;
		}
	} while( false );

	// 커넥션 실패
	if( !result )
	{
		_cprint( "Connecting to the upload server failed. Check upload server status. IP:%s / PORT:%d\n", ENV().GetString( "io.upload.ip", "127.0.0.1" ).c_str(), ENV().GetInt( "io.upload.port", 4517 ) );
		_cprint( "Enter 'connect upload' to retry.\n" );
		FILELOG( "Connecting to the upload server failed. Check upload server status. IP:%s / PORT:%d", ENV().GetString( "io.upload.ip", "127.0.0.1" ).c_str(), ENV().GetInt( "io.upload.port", 4517 ) );

		return false;
	}
	
	std::string strUploadConnectState = ENV().GetString( "upload.connect" );
	if( strUploadConnectState == "complete" )
		return true;

	// 로그인 시도, 로그인이 되지 않은 상태에서는 패킷을 날려도 알아서 무시
	void SendGameServerLoginToUpload( const char * szServerName );
	SendGameServerLoginToUpload( ENV().GetString( "app.name", "Unknown" ).c_str() );

	return true;
}

bool ConnectToCoordinator()
{
	extern IStreamSocketConnection * g_pCoordinatorConnection;

	// 이미 연결되어 있으면 패스
	if( g_pCoordinatorConnection && g_pCoordinatorConnection->IsConnected() )
		return true;

	bool result = true;
	do
	{
		static XIOCPConnection			coordinatorConnection( g_pIOCP->getOverlappedAllocator(), false );
		g_pCoordinatorConnection = static_cast< IStreamSocketConnection * >( &coordinatorConnection );

		coordinatorConnection.SetID( XOBJ_CONNECTION );

		if( !coordinatorConnection.SyncConnect( XAddr( ENV().GetString( "_coordinator.ip", "54.64.27.223" ).c_str(), ENV().GetInt( "_coordinator.port", 5000 ) ) ) )
		{
			result = false;
			break;
		}

		if( !g_pIOCP->AddObject( &coordinatorConnection ) )
		{
			result = false;
			break;
		}
	} while( false );

	// 커넥션 실패
	if( !result )
	{
		return false;
	}

	std::string strCoordinatorConnectState = ENV().GetString( "_coordinator.connect" );
	if( strCoordinatorConnectState == "complete" )
		return true;

	if( g_pCoordinatorConnection && g_pCoordinatorConnection->IsConnected() )
	{
		std::string str;
		XStringUtil::Format( str, "locale:%s,CodePage:%d,io.ip_address:%s,io.port:%d\n", XSTR( LOCALE ), ENV().GetInt( "CodePage" ), ENV().GetString( "io.ip_address" ).c_str(), ENV().GetInt( "io.port" ) );
		PendString( g_pCoordinatorConnection, str.c_str() );
	}

	return true;
}