
#include <stdexcept>
#include <ctime>

#include <network/IConnection.h>
#include <network/XIOCPConnection.h>
#include <mmo/ArRegion.h>
#include <toolkit/XEnv.h>
#include <toolkit/bits_scramble.h>
#include <toolkit/XConsole.h>
#include <mmo/ArcadiaServer.h>
#include <toolkit/XQuadScanner.h>
#include <toolkit/XStringUtil.h>
#include <toolkit/KHash.h>

#include "LogClient/LogClient.h"
#include "ErrorCode/ErrorCode.h"

#include "GameMessage.h"
#include "LUAVM.h"
#include "SendMessage.h"
#include "GameContent.h"
#include "StructPlayer.h"
#include "StructSkill.h"
#include "StructNPC.h"
#include "StructMonster.h"
#include "StructSummon.h"
#include "StructPet.h"
#include "StructFieldProp.h"
#include "DB_Commands.h"
#include "StructItem.h"
#include "PartyManager.h"
#include "GuildManager.h"
#include "DungeonManager.h"
#include "RankingManager.h"
#include "StructWorldLocation.h"
#include "StructSkillProp.h"
#include "ThreadPlayerHelper.h"


extern volatile LONG													g_nSendMessageCount;
extern volatile LONG													g_nSendMessageByte;
extern IStreamSocketConnection	*										g_pAuthConnection;
extern IStreamSocketConnection	*										g_pUploadConnection;
extern IStreamSocketConnection	*										g_pCoordinatorConnection;
extern bool																g_bUseMessageStatistics;

struct MessagingInfo
{
	unsigned short	msg_id;
	__int64			total_size;
	int				count;
};

struct MessageLogger : ArSchedulerObject
{
public:
	MessageLogger()
		: m_nLastLogTime( GetArTime() )
		, m_csLock( "MessageLogger::m_csLock" )
	{
	}

	void Init()
	{
		ArcadiaServer::Instance().SetObjectPriority( this, ArSchedulerObject::UPDATE_PRIORITY_LOW );
	}

	void DeInit()
	{
		ArcadiaServer::Instance().SetObjectPriority( this, ArSchedulerObject::UPDATE_PRIORITY_NULL );
	}

	void SetMessagingInfo( unsigned short msg_id, __int64 msg_size, bool bIsSendMessage )
	{
		//FILELOG("Outgoing Message: [ID: %d, Size: %d]", msg_id, msg_size);

		if( !g_bUseMessageStatistics )
			return;

		THREAD_SYNCRONIZE( m_csLock );

		std::vector< MessagingInfo > * pvInfo = &m_vSendInfo;

		if( !bIsSendMessage )
		{
			pvInfo = &m_vRecvInfo;
		}

		for( std::vector< MessagingInfo >::iterator it = pvInfo->begin(); it != pvInfo->end(); ++it )
		{
			if( (*it).msg_id == msg_id )
			{
				++(*it).count;
				(*it).total_size += msg_size;

				return;
			}
		}

		MessagingInfo _info;

		_info.msg_id = msg_id;
		_info.total_size = msg_size;
		_info.count = 1;

		pvInfo->push_back( _info );
	}

	void LogMessagingInfo()
	{
		if( !g_bUseMessageStatistics )
			return;

		std::string strLog;
		char buf[256];

		THREAD_SYNCRONIZE( m_csLock );

		strLog = "\r\n::Send Messaging Info::\r\n";

		for( std::vector< MessagingInfo >::iterator it = m_vSendInfo.begin(); it != m_vSendInfo.end(); ++it )
		{
			s_sprintf( buf, _countof( buf ), "%5u\t%20I64d\t%10d\r\n", (*it).msg_id, (*it).total_size, (*it).count );
			strLog += buf;
		}

		strLog += "\r\n::Recv Messaging Info::\r\n";

		for( std::vector< MessagingInfo >::iterator it = m_vRecvInfo.begin(); it != m_vRecvInfo.end(); ++it )
		{
			s_sprintf( buf, _countof( buf ), "%5u\t%20I64d\t%10d\r\n", (*it).msg_id, (*it).total_size, (*it).count );
			strLog += buf;
		}

		strLog += "\r\n\r\n";

		FileLogHandler::GetFileLogHandler()->LogStringEx( NULL, "MessagingLog", strLog.c_str() );

		m_vSendInfo.clear();
		m_vRecvInfo.clear();
	};

	virtual void onProcess( int nThreadNum )
	{
		char buf[255];
		s_sprintf( buf, _countof( buf ), "thread.scheduler.%d.proc", nThreadNum );
		ENV().Set( buf, "MessageLogger" );

		if( !g_bUseMessageStatistics )
			return;

		AR_TIME t = GetArTime();

		if( m_nLastLogTime + LOG_PERIOD < t )
		{
			m_nLastLogTime = t;

			LogMessagingInfo();
		}
	}

private:
	static const AR_TIME			LOG_PERIOD		= 600;
//	static const AR_TIME			LOG_PERIOD = 60000;
	XCriticalSection				m_csLock;
	std::vector< MessagingInfo >	m_vSendInfo;
	std::vector< MessagingInfo >	m_vRecvInfo;
	AR_TIME							m_nLastLogTime;
};

MessageLogger g_MessageLogger;

void InitMessageLogger()
{
	g_MessageLogger.Init();
}

void DeInitMessageLogger()
{
	g_MessageLogger.DeInit();
}

void SetMessagingInfo( unsigned short msg_id, __int64 msg_size, bool bIsSendMessage )
{
	g_MessageLogger.SetMessagingInfo( msg_id, msg_size, bIsSendMessage );
}

// 타임싱크
void SendTimeSync( StructPlayer * pPlayer )
{
	TS_TIMESYNC	msg;
	msg.time =  GetArTime();
	PendMessage( pPlayer, &msg );

#ifdef TIME_DEBUG
	_cprint( "SND : TM_TIMESYNC (%d)\n", msg.time );
#endif
}

void fillItemBaseInfo( TS_ITEM_BASE_INFO *pInfo, StructItem * pItem )
{
	assert(pInfo->Code != 3100003);

	pInfo->Code					= pItem->GetItemCode();
	pInfo->count				= pItem->GetCount();
	pInfo->Flag					= *pItem->GetInstanceFlag().GetRawData();
	pInfo->handle				= pItem->GetHandle();
	pInfo->ethereal_durability	= pItem->GetCurrentEtherealDurability();
	pInfo->endurance			= pItem->GetCurrentEndurance();
	pInfo->enhance				= pItem->GetItemEnhance();
	pInfo->enhance_chance		= pItem->GetEnhanceChance();
	pInfo->level				= pItem->GetItemLevel();
	pInfo->uid					= pItem->GetItemUID();

	if( pItem->IsExpireItem() )
		pInfo->remain_time	= pItem->GetExpireTime() - time( NULL );
	else
		pInfo->remain_time	= 0;

	for( unsigned i = 0; i < ItemBase::MAX_SOCKET_NUMBER; ++i )
	{
		pInfo->socket[i]	= pItem->GetSocketCode( i );
	}

	time_t tCurrent = time( NULL );

	if( pItem->GetElementalEffectType() && ( !pItem->GetElementalEffectExpireTime() || pItem->GetElementalEffectExpireTime() > tCurrent ) )
	{
		pInfo->elemental_effect_remain_time = pItem->GetElementalEffectExpireTime() ? pItem->GetElementalEffectExpireTime() - tCurrent : -1;
		pInfo->elemental_effect_type = pItem->GetElementalEffectType();
		pInfo->elemental_effect_attack_point = pItem->GetElementalEffectAttackPoint();
		pInfo->elemental_effect_magic_point = pItem->GetElementalEffectMagicPoint();
	}
	else
	{
		pInfo->elemental_effect_remain_time = 0;
		pInfo->elemental_effect_type = Elemental::TYPE_NONE;
		pInfo->elemental_effect_attack_point = 0;
		pInfo->elemental_effect_magic_point = 0;
	}

	pInfo->appearance_code = pItem->GetAppearanceCode();

	for( int nCnt = 0 ; nCnt < ItemInstance::MAX_AWAKEN_NUMBER ; ++nCnt )
	{
		pInfo->awakenoption.nValue1[nCnt] = pItem->GetAwakenOptionValue1( nCnt );
		pInfo->awakenoption.nValue2[nCnt] = pItem->GetAwakenOptionValue2( nCnt );
	}

	for( int nCnt = 0 ; nCnt < ItemInstance::MAX_RANDOM_OPTION_NUMBER ; ++nCnt )
	{
		pInfo->random_option.nType[nCnt] = pItem->GetIdentifiedOptionType( nCnt );
		pInfo->random_option.fValue1[nCnt] = pItem->GetIdentifiedOptionValue1( nCnt );
		pInfo->random_option.fValue2[nCnt] = pItem->GetIdentifiedOptionValue2( nCnt );
	}

#ifdef USE_CLIENT97
	pInfo->effect_id = pItem->GetEffectID();
#endif

	if( pItem->IsSummonCard() )
	{
		StructSummon * pSummon = pItem->GetSummonStruct();

		if( pSummon )
		{
			int depth = pSummon->GetTransformLevel();
			int i;
			for( i = 0; i < depth - 1; ++i )
			{
				pInfo->socket[i + 1] = pSummon->GetPrevJobLevel( i );
			}
			pInfo->socket[i + 1] = pSummon->GetLevel();
		}
	}

	pInfo->summon_code = pItem->GetSummonCode();
}

void fillItemInfo( TS_ITEM_INFO *pInfo, StructItem *pItem )
{
	fillItemBaseInfo( pInfo, pItem );

	pInfo->wear_position = pItem->GetWearInfo();

	pInfo->own_summon_handle = 0;

	if( pItem->GetOwnSummonUID() > 0 )
	{
		pInfo->own_summon_handle = pItem->GetOwnSummonHandle();

	}

	if( pItem->IsInStorage() )
	{
		pInfo->wear_position = -2;
	}

	pInfo->index = pItem->GetIdx();
}

void SendDropResult( struct StructPlayer * pClient, AR_HANDLE ItemHandle, bool bIsSuccess )
{
	TS_SC_DROP_RESULT msg;
	msg.item_handle = ItemHandle;
	msg.isAccepted = bIsSuccess;
	PendMessage( pClient, &msg );
}

void SendTakeResult( struct StructPlayer * pClient, struct StructItem *pItem )
{
	/*
	TS_SC_TAKE_RESULT msg;
	fillItemInfo( &msg.item_info, pItem );
	PendMessage( pClient, &msg );
	*/
}


static void pendMessage( struct IStreamSocketConnection *pConn, const struct TS_MESSAGE * pMessage )
{
	if( !pConn ) return;

	TS_MESSAGE* temp = const_cast< TS_MESSAGE* >( pMessage );
	temp->set_check_sum();

	InterlockedIncrement( &g_nSendMessageCount );
	InterlockedExchangeAdd( &g_nSendMessageByte, pMessage->size );

	SetMessagingInfo( pMessage->id, pMessage->size, true );

	int nSize = pConn->Write( pMessage, pMessage->size );
	
	if( nSize != pMessage->size ) 
	{
		//assert( 0 );
		return;
	}
}

static void pendStream( struct IStreamSocketConnection *pConn, const void *pBuffer, unsigned size )
{
	if( !pConn ) return;

	InterlockedIncrement( &g_nSendMessageCount );
	InterlockedExchangeAdd( &g_nSendMessageByte, size );

	const struct TS_MESSAGE * pMessage = static_cast< const struct TS_MESSAGE * >( pBuffer );

	SetMessagingInfo( pMessage->id, pMessage->size, true );

	int nSize = pConn->Write( pBuffer, size );
	
	if( nSize != size ) 
	{
		//assert( 0 );
		return;
	}
}

static void pendString( struct IStreamSocketConnection *pConn, const char *szString, size_t nLength )
{
	if( !pConn ) return;
	int nSize = pConn->Write( szString, nLength );
}

void PendMessage( struct IStreamSocketConnection *pConn, const struct TS_MESSAGE * pMessage )
{
	//if( LockIOCPConnection( pConn ) )
	{
		pendMessage( pConn, pMessage );
		//UnLockIOCPConnection( pConn );
	}
}

void PendMessage( const struct ArObject *pObj, const struct TS_MESSAGE * pMessage )
{
	if( static_cast< const GameObject * >(pObj)->IsPlayer() && static_cast< const StructPlayer* >( pObj )->pConnection && static_cast< const StructPlayer* >( pObj )->pConnection->IsConnected() )
	{
		pendMessage( static_cast< const StructPlayer* >( pObj )->pConnection, pMessage );
	}
}

void PendStream( const struct ArObject *pObj, const void *pBuffer, unsigned size )
{
	if( static_cast< const GameObject * >(pObj)->IsPlayer() && static_cast< const StructPlayer* >( pObj )->pConnection && static_cast< const StructPlayer* >( pObj )->pConnection->IsConnected() )
	{
		pendStream(	static_cast< const StructPlayer* >( pObj )->pConnection, pBuffer, size );
	}
}

void PendString( struct IStreamSocketConnection *pConnection, const char *szStr )
{
	pendString( pConnection, szStr, strlen( szStr ) );
}

void SendResult( const struct ArObject *pObj, unsigned short msg, unsigned short result, int value )
{
	if( static_cast< const GameObject * >(pObj)->IsPlayer() && static_cast< const StructPlayer* >( pObj )->pConnection && static_cast< const StructPlayer* >( pObj )->pConnection->IsConnected() )
	{
		pendMessage( static_cast< const StructPlayer* >( pObj )->pConnection, &TS_SC_RESULT( msg, result, value ) );
	}
}

void SendResult( IStreamSocketConnection *pConnection, unsigned short msg, unsigned short result, int value )
{
	//if( LockIOCPConnection( pConnection ) )
	if( pConnection && pConnection->IsConnected() )
	{
		pendMessage( pConnection, &TS_SC_RESULT( msg, result, value ) );
		//UnLockIOCPConnection( pConnection );
	}
}

void PrintfChatMessage( bool bLock, int nChatType, const char *szSenderName, struct StructPlayer *pTarget, const char *szString, ... )
{
	char szBuf[2048];
	va_list va;
	va_start( va, szString );
	s_vsprintf( szBuf, _countof(szBuf), szString, va );
	va_end( va );

	SendChatMessage( bLock, nChatType, szSenderName, pTarget, szBuf, (unsigned)strlen( szBuf ) );
}

void SendSelfChatMessage( int nChatType, const char *szSenderName, struct StructPlayer *pTarget, const char *szString, unsigned len )
{
	SendChatMessage( false, nChatType, szSenderName,pTarget->GetHandle(), szString, len );
}

/*
void PrintfChatMessage( const char *szSenderName, struct StructPlayer *pTarget, const char *szString, ... )
{
	char szBuf[2048];
	va_list va;
	va_start( va, szString );
	s_vsprintf( szBuf, _countof(szBuf), szString, va );
	va_end( va );

	SendChatMessage( false, szSenderName, pTarget, szBuf );
}*/

void SendQuestMessage( int nChatType, struct StructPlayer *pTarget, const char *szString, ... )
{
	char szBuf[2048];
	va_list va;
	va_start( va, szString );
	s_vsprintf( szBuf, _countof(szBuf), szString, va );
	va_end( va );

	SendChatMessage( true, nChatType, "@QUEST", pTarget, szBuf );
}

void PrintfGlobalChatMessage( int nChatType, const char *szSenderName, const char *szString, ... )
{
	char szBuf[2048];
	va_list va;
	va_start( va, szString );
	s_vsprintf( szBuf, _countof(szBuf), szString, va );
	va_end( va );

	SendGlobalChatMessage( nChatType, szSenderName, szBuf );
}

void SendPartyChatMessage( int nChatType, const char *szSender, int nPartyID, const char *szString, unsigned len )
{
	struct myPartyFunctor : PartyManager::PartyFunctor
	{
		myPartyFunctor( const char *s, const char *p, int t ) : szSender( s ), pString( p ), nChatType( t ) {}

		virtual bool operator()( AR_HANDLE handle )
		{
			SendChatMessage( false, nChatType, szSender, handle, pString );

			return true;
		}
		int nChatType;
		const char *szSender;
		const char *pString;
	} _fo( szSender, szString, nChatType );

	PartyManager::GetInstance().DoEachMember( nPartyID, _fo );
}

void SendPartyChatMessage( const char *szSender, int nPartyID, const char *szString, unsigned len )
{
	SendPartyChatMessage( CHAT_PARTY, szSender, nPartyID, szString, len );
}

void PrintfPartyChatMessage( int nChatType, int nPartyID, const char *szString, ... )
{
	char szBuf[2048];
	va_list va;
	va_start( va, szString );
	s_vsprintf( szBuf, _countof(szBuf), szString, va );
	va_end( va );

	SendPartyChatMessage( nChatType, "@PARTY", nPartyID, szBuf );
}

void PrintfPartyChatMessage( int nPartyID, const char *szString, ... )
{
	char szBuf[2048];
	va_list va;
	va_start( va, szString );
	s_vsprintf( szBuf, _countof(szBuf), szString, va );
	va_end( va );

	SendPartyChatMessage( "@PARTY", nPartyID, szBuf );
}

void SendGuildChatMessage( int nChatType, const char *szSender, int nGuildID, const char *szString, unsigned len )
{
	struct myGuildFunctor : GuildManager::GuildFunctor
	{
		myGuildFunctor( const char *s, const char *p, int t ) : szSender( s ), pString( p ), nChatType( t ) {}

		virtual bool operator()( AR_HANDLE handle )
		{
			SendChatMessage( false, nChatType, szSender, handle, pString );

			return true;
		}
		int nChatType;
		const char *szSender;
		const char *pString;
	} _fo( szSender, szString, nChatType );

	GuildManager::GetInstance().DoEachMember( nGuildID, _fo );
}

void SendGuildChatMessage( const char *szSender, int nGuildID, const char *szString, unsigned len )
{
	SendGuildChatMessage( CHAT_GUILD, szSender, nGuildID, szString, len );
}

void PrintfGuildChatMessage( int nChatType, const char * szSender, int nGuildID, const char *szString, ... )
{
	char szBuf[2048];
	va_list va;
	va_start( va, szString );
	s_vsprintf( szBuf, _countof(szBuf), szString, va );
	va_end( va );

	SendGuildChatMessage( nChatType, szSender, nGuildID, szBuf );
}

void PrintfGuildChatMessage( int nChatType, int nGuildID, const char *szString, ... )
{
	char szBuf[2048];
	va_list va;
	va_start( va, szString );
	s_vsprintf( szBuf, _countof(szBuf), szString, va );
	va_end( va );

	SendGuildChatMessage( nChatType, "@GUILD", nGuildID, szBuf );
}

void PrintfGuildChatMessage( int nGuildID, const char *szString, ... )
{
	char szBuf[2048];
	va_list va;
	va_start( va, szString );
	s_vsprintf( szBuf, _countof(szBuf), szString, va );
	va_end( va );

	SendGuildChatMessage( "@GUILD", nGuildID, szBuf );
}

void PrintfAllianceChatMessage( int nChatType, int nAllianceID, const char * szSender, const char * szString, ... )
{
	char szBuf[2048];
	va_list va;
	va_start( va, szString );
	s_vsprintf( szBuf, _countof(szBuf), szString, va );
	va_end( va );

	std::vector< int > vAllianceMember = GuildManager::GetInstance().GetAllianceMemberID( nAllianceID );

	for( std::vector< int >::iterator it = vAllianceMember.begin(); it != vAllianceMember.end(); ++it )
	{
		SendGuildChatMessage( nChatType, szSender, (*it), szBuf );
	}
}

void PrintfAllianceChatMessage( int nAllianceID, const char * szSender, const char * szString, ... )
{
	char szBuf[2048];
	va_list va;
	va_start( va, szString );
	s_vsprintf( szBuf, _countof(szBuf), szString, va );
	va_end( va );

	PrintfAllianceChatMessage( CHAT_ALLIANCE_SYSTEM, nAllianceID, szSender, szBuf );
}

void PrintfAllianceChatMessage( int nAllianceID, const char * szString, ... )
{
	char szBuf[2048];
	va_list va;
	va_start( va, szString );
	s_vsprintf( szBuf, _countof(szBuf), szString, va );
	va_end( va );

	PrintfAllianceChatMessage( nAllianceID, "@ALLIANCE", szBuf );
}

void SendLocalChatMessage( int nChatType, unsigned rx, unsigned ry, unsigned char layer, const char *szSenderName, const char *szString, unsigned len )
{
	ARCADIA_LOCK( ArcadiaServer::Instance().LockWithVisibleRange( rx, ry, layer ) );
	// 메세지 헤더
	TS_SC_CHAT msg;
	msg.type = nChatType;
	s_strcpy( msg.szSender, _countof( msg.szSender ), szSenderName );
	msg.len = (unsigned char)(len + 1);
	
	// 문자열 크기만큼 사이즈 늘림
	msg.size += static_cast< unsigned >( msg.len );

	if ( msg.size > SENDMSG_BUFFER_SIZE )
	{
		FILELOG( "SendLocalChatMessage too big : %d", msg.size );
		_cprint( "SendLocalChatMessage too big : %d", msg.size );
		return;
	}
	// 메모리 할당
	char pBuffer[ SENDMSG_BUFFER_SIZE ];
// char * pBuffer = new char[ msg.size ];


	// 버퍼에 헤더 복사
	s_memcpy( pBuffer, sizeof( pBuffer ), &msg, sizeof(msg) );
	
	// 버퍼에 문자열 복사
	s_memcpy( pBuffer + sizeof(msg), sizeof( pBuffer )-sizeof(msg), szString, msg.len );

	// '\0' 추가
	pBuffer[ msg.size-1 ] = '\0';

	// 메세지 전송
	ArcadiaServer::Instance().Broadcast( rx, ry, layer, pBuffer );
	
}

void BroadcastChatMessage( int nChatType, unsigned rx, unsigned ry, unsigned char layer, const char *szSenderName, const char *szString, unsigned len )
{
	// 메세지 헤더
	TS_SC_CHAT msg;
	msg.type = nChatType;
	s_strcpy( msg.szSender, _countof( msg.szSender ), szSenderName );
	msg.len = (unsigned char)(len + 1);
	
	// 문자열 크기만큼 사이즈 늘림
	msg.size += static_cast< unsigned >( msg.len );

	if ( msg.size > SENDMSG_BUFFER_SIZE )
	{
		FILELOG( "BroadcastChatMessage too big : %d", msg.size );
		_cprint( "BroadcastChatMessage too big : %d", msg.size );
		return;
	}

	// 메모리 할당
	char pBuffer[SENDMSG_BUFFER_SIZE];

	// 버퍼에 헤더 복사
	s_memcpy( pBuffer, sizeof( pBuffer ), &msg, sizeof(msg) );
	
	// 버퍼에 문자열 복사
	s_memcpy( pBuffer + sizeof(msg), sizeof( pBuffer ) - sizeof(msg), szString, msg.len );

	// '\0' 추가
	pBuffer[ msg.size-1 ] = '\0';

	// 메세지 전송
	ArcadiaServer::Instance().Broadcast( rx, ry, layer, pBuffer );
}

void PrintfLocalChatMessage( bool bLock, int ChatType, unsigned rx, unsigned ry, unsigned char layer, const char *szSenderName, const char * szString, ... )
{
	char szBuf[2048];
	va_list va;
	va_start( va, szString );
	int nResultLen = s_vsprintf( szBuf, _countof(szBuf), szString, va );
	va_end( va );

	if( nResultLen <= 0 )
		return;

	if( bLock )
		SendLocalChatMessage( ChatType, rx, ry, layer, szSenderName, szBuf, nResultLen );
	else
		BroadcastChatMessage( ChatType, rx, ry, layer, szSenderName, szBuf, nResultLen );
}

void SendGuildNotify( struct StructPlayer *pPlayer )
{
	char szBuf[256];

	s_sprintf( szBuf, _countof( szBuf ), "GCHANGE|%u|%d", pPlayer->GetHandle(), pPlayer->GetGuildID() );

	SendLocalChatMessage( CHAT_GUILD_SYSTEM, pPlayer->GetRX(), pPlayer->GetRY(), pPlayer->GetLayer(), "@GUILD", szBuf, static_cast<unsigned int>( strlen( szBuf ) ) + 1 );
}

void SendGlobalChatMessage( int ChatType, const char *szSenderName, const char *szString, unsigned len )
{
	if( !len ) len = (unsigned)strlen( szString );

	// 메세지 헤더
	TS_SC_CHAT msg;
	msg.type = ChatType;
	s_strcpy( msg.szSender, _countof( msg.szSender ), szSenderName );
	msg.len = (unsigned short)(len + 1);
	
	// 문자열 크기만큼 사이즈 늘림
	msg.size += static_cast< unsigned >( msg.len );

	if ( msg.size > SENDMSG_BUFFER_SIZE )
	{
		FILELOG( "SendGlobalChatMessage too big : %d", msg.size );
		_cprint( "SendGlobalChatMessage too big : %d", msg.size );
		return;
	}

	// 메모리 할당
	char pBuffer[SENDMSG_BUFFER_SIZE];

	// 버퍼에 헤더 복사
	s_memcpy( pBuffer, sizeof( pBuffer ), &msg, sizeof(msg) );
	
	// 버퍼에 문자열 복사
	s_memcpy( pBuffer + sizeof(msg), sizeof( pBuffer ) - sizeof( msg ), szString, msg.len );

	// '\0' 추가
	pBuffer[ msg.size-1 ] = '\0';

	// 메세지 전송
	struct _ChatSender : ArObjectFunctor
	{
		_ChatSender( const void *_msg ) { msg = _msg; }

		void operator()( ArObject *pObj ) const
		{
			PendMessage( pObj,
						 reinterpret_cast< const TS_MESSAGE* >( msg ) );
		}

		const void *msg;

	} _sender( pBuffer );

	StructPlayer::DoEachPlayer( _sender );

	StructPlayer::iterator pit( StructPlayer::get( StructPlayer::FindPlayer( szSenderName ) ) );
	StructPlayer *pSender = *pit;
	if( pSender )
	{
		SendResult( pSender, TM_CS_CHAT_REQUEST, RESULT_SUCCESS );
	}
}

void SendWarpMessage( const StructPlayer *pPlayer )
{
	TS_SC_WARP msg;
	msg.x = pPlayer->GetX();
	msg.y = pPlayer->GetY();
	msg.z = pPlayer->GetZ();
	msg.layer = pPlayer->GetLayer();

	PendMessage( pPlayer, &msg );
}

void SendChatMessageWithLock( AR_HANDLE hTarget, const void *pMsg, unsigned len )
{
	struct _MessageSender : ArObjectFunctor
	{
		_MessageSender( const void *_msg, unsigned int _size ) : msg( _msg ), size( _size )	{}

		void operator()( ArObject *pObj ) const
		{
			PendStream( pObj, msg, size );
		}
		
		unsigned int size;
		const void *msg;

	} _sender( pMsg, len );

	StructPlayer::DoPlayer( hTarget, _sender );	
}

void SendChatMessageWithoutLock( AR_HANDLE hTarget, void *pBuffer, unsigned len )
{
	StructPlayer::iterator pit( StructPlayer::get( hTarget ) );
	StructPlayer *pPlayer = *pit;

	TS_SC_CHAT *pMsg = reinterpret_cast< TS_SC_CHAT * >( pBuffer );

	if( pPlayer ) PendStream( pPlayer, pMsg, len );
}

void SendChatMessage( bool bLock, int nChatType, const char *szSenderName, StructPlayer *pTarget, const char *szString, unsigned len )
{
	SendChatMessage( bLock, nChatType, szSenderName, pTarget->GetHandle(), szString, len );
}

void SendChatMessage( bool bLock, int nChatType, const char *szSenderName, AR_HANDLE hTarget, const char *szString, unsigned len )
{
	if( !len ) len = (unsigned)strlen( szString );

	if( len > 30000 )
	{
		//assert( 0 );
		return;
	}

	// 메세지 헤더
	TS_SC_CHAT msg;
	msg.type = nChatType;
	s_strcpy( msg.szSender, _countof( msg.szSender ), szSenderName );
	msg.len = (unsigned short)(len + 1);
	
	// 문자열 크기만큼 사이즈 늘림
	msg.size += static_cast< unsigned >( msg.len );

	if ( msg.size > SENDMSG_BUFFER_SIZE )
	{
		FILELOG( "SendChatMessage too big : %d", msg.size );
		_cprint( "SendChatMessage too big : %d", msg.size );
		return;
	}

	// 메모리 할당
	char pBuffer[SENDMSG_BUFFER_SIZE];

	// 버퍼에 헤더 복사
	s_memcpy( pBuffer, sizeof( pBuffer ), &msg, sizeof(msg) );
	
	// 버퍼에 문자열 복사
	s_memcpy( pBuffer + sizeof(msg), sizeof( pBuffer ) - sizeof( msg ), szString, msg.len );

	// '\0' 추가
	pBuffer[ msg.size-1 ] = '\0';

	if( bLock ) SendChatMessageWithLock( hTarget, pBuffer, msg.size );
	else		SendChatMessageWithoutLock( hTarget, pBuffer, msg.size );
}

void SendChatMessage( bool bLock, int nChatType, const char *szSenderName, const char *szTarget, const char *szString, unsigned len )
{
	if( !len ) len = (unsigned)strlen( szString );

	StructPlayer::iterator pit_sender( StructPlayer::get( StructPlayer::FindPlayer( szSenderName ) ) );
	StructPlayer *pSender = *pit_sender;
	StructPlayer::iterator pit_target( StructPlayer::get( StructPlayer::FindPlayer( szTarget ) ) );
	StructPlayer *pTarget = *pit_target;
	
	if( !pTarget )
	{
		if( pSender )
			SendResult( pSender, TM_CS_CHAT_REQUEST, RESULT_NOT_EXIST );

		return;
	}

	SendChatMessage( bLock, nChatType, szSenderName, pTarget->GetHandle(), szString, len );

	if( !pSender ) ENV().Set( "error_chat", 1 );

	if( pSender )
		SendResult( pSender, TM_CS_CHAT_REQUEST, RESULT_SUCCESS );
}

void SendYellChatMessage( int nChatType, AR_HANDLE handle, const char *szString, unsigned len )
{
	StructCreature::iterator cit = StructCreature::get( handle );
	if( !(*cit) )
	{
		FILELOG( "SendYellChatMessage invalid object : %I64d", handle );
		_cprint( "SendYellChatMessage invalid object : %I64d\n", handle );
		return;
	}

	ARCADIA_LOCK( ArcadiaServer::Instance().LockObjectWithSpecificRange( cit.get(), GameRule::CHAT_YELL_RANGE ) );

	// 메세지 헤더
	TS_SC_CHAT_LOCAL msg;
	msg.type = nChatType;
	msg.handle = handle;
	msg.len = (unsigned char)(len + 1);
	
	// 문자열 크기만큼 사이즈 늘림
	msg.size += static_cast< unsigned >( msg.len );

	if ( msg.size > SENDMSG_BUFFER_SIZE )
	{
		FILELOG( "SendYellChatMessage too big : %d", msg.size );
		_cprint( "SendYellChatMessage too big : %d", msg.size );
		return;
	}
	// 메모리 할당
	char pBuffer[SENDMSG_BUFFER_SIZE];

	// 버퍼에 헤더 복사
	s_memcpy( pBuffer, sizeof( pBuffer ), &msg, sizeof(msg) );
	
	// 버퍼에 문자열 복사
	s_memcpy( pBuffer + sizeof(msg), sizeof( pBuffer ) - sizeof( msg ), szString, msg.len );

	// '\0' 추가
	pBuffer[ msg.size-1 ] = '\0';

	// 메세지 전송
	ArcadiaServer::Instance().BroadcastToSpecificRegion( cit.get()->GetRX(), cit.get()->GetRY(), GameRule::CHAT_YELL_RANGE, cit.get()->GetLayer(), pBuffer );
	
	StructPlayer::iterator pit( StructPlayer::get( handle ) );
	StructPlayer *pPlayer = *pit;
	if( pPlayer )
	{
		SendResult( pPlayer, TM_CS_CHAT_REQUEST, RESULT_SUCCESS );
	}
}

void SendLocalChatMessage( int nChatType, AR_HANDLE handle, const char *szString, unsigned len )
{	
	StructCreature::iterator cit = StructCreature::get( handle );
	if( !(*cit) )
	{
		FILELOG( "SendLocalChatMessage invalid object : %I64d", handle );
		_cprint( "SendLocalChatMessage invalid object : %I64d\n", handle );
		return;
	}

	ARCADIA_LOCK( ArcadiaServer::Instance().LockObjectWithVisibleRange( cit.get() ) );

	// 메세지 헤더
	TS_SC_CHAT_LOCAL msg;
	msg.type = nChatType;
	msg.handle = handle;
	msg.len = (unsigned char)(len + 1);
	
	// 문자열 크기만큼 사이즈 늘림
	msg.size += static_cast< unsigned >( msg.len );

	if ( msg.size > SENDMSG_BUFFER_SIZE )
	{
		FILELOG( "SendLocalChatMessage too big : %d", msg.size );
		_cprint( "SendLocalChatMessage too big : %d", msg.size );
		return;
	}
	// 메모리 할당

	char pBuffer[SENDMSG_BUFFER_SIZE];

	// 버퍼에 헤더 복사
	s_memcpy( pBuffer, sizeof( pBuffer ), &msg, sizeof(msg) );
	
	// 버퍼에 문자열 복사
	s_memcpy( pBuffer + sizeof(msg), sizeof( pBuffer ) - sizeof( msg ), szString, msg.len );

	// '\0' 추가
	pBuffer[ msg.size-1 ] = '\0';

	// 메세지 전송
	ArcadiaServer::Instance().Broadcast( cit.get()->GetRX(), cit.get()->GetRY(), cit.get()->GetLayer(), pBuffer );
	
	StructPlayer::iterator pit( StructPlayer::get( handle ) );
	StructPlayer *pPlayer = *pit;
	if( pPlayer && pPlayer->IsPlayer() )
	{
		SendResult( pPlayer, TM_CS_CHAT_REQUEST, RESULT_SUCCESS );
	}
}

void BroadcastTamingMessage( struct StructPlayer *pPlayer, struct StructCreature *pMonster, int mode )
{
	if( !pPlayer )
		return;

	TS_SC_TAMING_INFO info;
	info.tamer_handle	= pPlayer ? pPlayer->GetHandle() : 0;
	info.target_handle	= pMonster->GetHandle();
	info.mode			= mode;
	ArcadiaServer::Instance().Broadcast( pMonster->GetRX(), pMonster->GetRY(), pMonster->GetLayer(), &info );

	switch( mode )
	{
	case TS_SC_TAMING_INFO::START:
		if( pPlayer )
		{
			if( pPlayer->GetPartyID() )
				PrintfPartyChatMessage( CHAT_PARTY_SYSTEM, pPlayer->GetPartyID(), "TAMING_START|%s|", pMonster->GetName() );
			else
				PrintfChatMessage( false, CHAT_PARTY_SYSTEM, "@SYSTEM", pPlayer, "TAMING_START|%s|", pMonster->GetName() );
		}
		break;
	case TS_SC_TAMING_INFO::CLEAR:
	case TS_SC_TAMING_INFO::FAILED:
		if( pPlayer )
		{
			if( pPlayer->GetPartyID() )
				PrintfPartyChatMessage( CHAT_PARTY_SYSTEM, pPlayer->GetPartyID(), "TAMING_FAILED|%s|", pMonster->GetName() );
			else
				PrintfChatMessage( false, CHAT_PARTY_SYSTEM, "@SYSTEM", pPlayer, "TAMING_FAILED|%s|", pMonster->GetName() );
		}
		break;
	case TS_SC_TAMING_INFO::SUCCESS:
		if( pPlayer )
		{
			if( pPlayer->GetPartyID() )
				PrintfPartyChatMessage( CHAT_PARTY_SYSTEM, pPlayer->GetPartyID(), "TAMING_SUCCESS|%s|", pMonster->GetName() );
			else
				PrintfChatMessage( false, CHAT_PARTY_SYSTEM, "@SYSTEM", pPlayer, "TAMING_SUCCESS|%s|", pMonster->GetName() );
		}
		break;
	}
}

template< int FACTOR >
const short RoundUp( const c_fixed< FACTOR > fixed )
{
	c_fixd< FACTOR > temp;
	temp.set( fixed.get() + FACTOR - 1 );

}

void SendStatInfo( struct StructPlayer * pClient, struct StructCreature * pCreature )
{
	{
		TS_SC_STAT_INFO	msg;

		// 기본 정보 전송
		msg.handle		= pCreature->GetHandle();

		const CreatureStatServer & stat = pCreature->GetStat();
		msg.stat.stat_id = stat.stat_id;
		msg.stat.strength		= stat.strength;
		msg.stat.vital			= stat.vital;
		msg.stat.dexterity		= stat.dexterity;
		msg.stat.agility		= stat.agility;
		msg.stat.intelligence	= stat.intelligence;
		msg.stat.mentality		= stat.mentality;
		msg.stat.luck			= stat.luck;

		const CreatureAttributeServer & attr = pCreature->GetAttribute();
		msg.attribute.nCritical					= attr.fCritical;
		msg.attribute.nCriticalPower			= attr.fCriticalPower;
		msg.attribute.nAttackPointLeft			= attr.fAttackPointLeft;
		msg.attribute.nAttackPointRight			= attr.fAttackPointRight;
		msg.attribute.nDefence					= attr.fDefence;
		msg.attribute.nBlockDefence				= attr.fBlockDefence;
		msg.attribute.nMagicPoint				= attr.fMagicPoint;
		msg.attribute.nMagicDefence				= attr.fMagicDefence;
		msg.attribute.nAccuracyRight			= attr.fAccuracyRight;
		msg.attribute.nAccuracyLeft				= attr.fAccuracyLeft;
		msg.attribute.nMagicAccuracy			= attr.fMagicAccuracy;
		msg.attribute.nAvoid					= attr.fAvoid;
		msg.attribute.nMagicAvoid				= attr.fMagicAvoid;
		msg.attribute.nBlockChance				= attr.fBlockChance;

		msg.attribute.nMoveSpeed				= attr.fMoveSpeed;
		msg.attribute.nAttackSpeed				= attr.fAttackSpeed;
		msg.attribute.nAttackRange				= attr.fAttackRange;
		
		msg.attribute.nMaxWeight				= attr.fMaxWeight;
		msg.attribute.nCastingSpeed				= attr.fCastingSpeed;
		msg.attribute.nCoolTimeSpeed			= attr.fCoolTimeSpeed;
		msg.attribute.nItemChance				= attr.fItemChance;
		msg.attribute.nHPRegenPercentage		= attr.fHPRegenPercentage;
		msg.attribute.nHPRegenPoint				= attr.fHPRegenPoint;
		msg.attribute.nMPRegenPercentage		= attr.fMPRegenPercentage;
		msg.attribute.nMPRegenPoint				= attr.fMPRegenPoint;
		msg.attribute.nPerfectBlock				= attr.fPerfectBlock;
		msg.attribute.nMagicalDefIgnore			= attr.fMagicalDefIgnore;
		msg.attribute.nMagicalDefIgnoreRatio	= attr.fMagicalDefIgnoreRatio;
		msg.attribute.nPhysicalDefIgnore		= attr.fPhysicalDefIgnore;
		msg.attribute.nPhysicalDefIgnoreRatio	= attr.fPhysicalDefIgnoreRatio;
#ifndef USE_CLIENT97
		msg.attribute.nMagicalPenetration		= attr.fMagicalPenetration;
		msg.attribute.nMagicalPenetrationRatio	= attr.fMagicalPenetrationRatio;
		msg.attribute.nPhysicalPenetration		= attr.fPhysicalPenetration;
		msg.attribute.nPhysicalPenetrationRatio	= attr.fPhysicalPenetrationRatio;
#endif
//		msg.attribute.nAttackRange = 1000;
//		msg.attribute.nCoolTimeSpeed = 1300;				// 재시전속도	 

		msg.type		= TS_SC_STAT_INFO::TOTAL;
		PendMessage( pClient, &msg );

		// 아이템빨 정보 전송
		msg.handle		= pCreature->GetHandle();

		// 원본 스탯의 정수 값을 구한 뒤 이를 최종 스탯의 정수값에서 빼서 보너스 값을 구한다

		// 단순 대입하지 않고 아래와 같이 구현한 까닭은 소수점 이하 값이 절삭되면 원본 스탯이 가끔 왜곡되어 나오기 때문
		// 가장 좋은 방법은 애초에 원본 스탯+최종 스탯을 보내주는 것이지만 클라의 변경이 동반되므로 일단 보류
		// 보너스 스탯에 대해 올림을 하는 방법도 있으나 이 경우는 똑같은 결과임에도 분기문이 들어가야 해서 기각
		const CreatureStatServer & statByState = pCreature->GetStatByState();
		msg.stat.stat_id		= statByState.stat_id;
		msg.stat.strength		-= static_cast< int >( stat.strength - statByState.strength );
		msg.stat.vital			-= static_cast< int >( stat.vital - statByState.vital );
		msg.stat.dexterity		-= static_cast< int >( stat.dexterity - statByState.dexterity );
		msg.stat.agility		-= static_cast< int >( stat.agility - statByState.agility );
		msg.stat.intelligence	-= static_cast< int >( stat.intelligence - statByState.intelligence );
		msg.stat.mentality		-= static_cast< int >( stat.mentality - statByState.mentality );
		msg.stat.luck			-= static_cast< int >( stat.luck - statByState.luck );

		const CreatureAttributeServer & attrByState = pCreature->GetAttributeByState();
		msg.attribute.nCritical					-= static_cast< int >( attr.fCritical - attrByState.fCritical );
		msg.attribute.nCriticalPower			-= static_cast< int >( attr.fCriticalPower - attrByState.fCriticalPower );
		msg.attribute.nAttackPointRight			-= static_cast< int >( attr.fAttackPointRight - attrByState.fAttackPointRight );
		msg.attribute.nAttackPointLeft			-= static_cast< int >( attr.fAttackPointLeft - attrByState.fAttackPointLeft );
		msg.attribute.nDefence					-= static_cast< int >( attr.fDefence - attrByState.fDefence );
		msg.attribute.nBlockDefence				-= static_cast< int >( attr.fBlockDefence - attrByState.fBlockDefence );
		msg.attribute.nMagicPoint				-= static_cast< int >( attr.fMagicPoint - attrByState.fMagicPoint );
		msg.attribute.nMagicDefence				-= static_cast< int >( attr.fMagicDefence - attrByState.fMagicDefence );
		msg.attribute.nAccuracyRight			-= static_cast< int >( attr.fAccuracyRight - attrByState.fAccuracyRight );
		msg.attribute.nAccuracyLeft				-= static_cast< int >( attr.fAccuracyLeft - attrByState.fAccuracyLeft );
		msg.attribute.nMagicAccuracy			-= static_cast< int >( attr.fMagicAccuracy - attrByState.fMagicAccuracy );
		msg.attribute.nAvoid					-= static_cast< int >( attr.fAvoid - attrByState.fAvoid );
		msg.attribute.nMagicAvoid				-= static_cast< int >( attr.fMagicAvoid - attrByState.fMagicAvoid );
		msg.attribute.nBlockChance				-= static_cast< int >( attr.fBlockChance - attrByState.fBlockChance );
		msg.attribute.nMoveSpeed				-= static_cast< int >( attr.fMoveSpeed - attrByState.fMoveSpeed );
		msg.attribute.nAttackSpeed				-= static_cast< int >( attr.fAttackSpeed - attrByState.fAttackSpeed );
		msg.attribute.nAttackRange				-= static_cast< int >( attr.fAttackRange - attrByState.fAttackRange );
		msg.attribute.nMaxWeight				-= static_cast< int >( attr.fMaxWeight - attrByState.fMaxWeight );
		msg.attribute.nCastingSpeed				-= static_cast< int >( attr.fCastingSpeed - attrByState.fCastingSpeed );
		msg.attribute.nCoolTimeSpeed			-= static_cast< int >( attr.fCoolTimeSpeed - attrByState.fCoolTimeSpeed );
		msg.attribute.nItemChance				-= static_cast< int >( attr.fItemChance - attrByState.fItemChance );
		msg.attribute.nHPRegenPercentage		-= static_cast< int >( attr.fHPRegenPercentage - attrByState.fHPRegenPercentage );
		msg.attribute.nHPRegenPoint				-= static_cast< int >( attr.fHPRegenPoint - attrByState.fHPRegenPoint );
		msg.attribute.nMPRegenPercentage		-= static_cast< int >( attr.fMPRegenPercentage - attrByState.fMPRegenPercentage );
		msg.attribute.nMPRegenPoint				-= static_cast< int >( attr.fMPRegenPoint - attrByState.fMPRegenPoint );
		msg.attribute.nPerfectBlock				-= static_cast< int >( attr.fPerfectBlock - attrByState.fPerfectBlock );
		msg.attribute.nMagicalDefIgnore			-= static_cast< int >( attr.fMagicalDefIgnore - attrByState.fMagicalDefIgnore );
		msg.attribute.nMagicalDefIgnoreRatio	-= static_cast< int >( attr.fMagicalDefIgnoreRatio - attrByState.fMagicalDefIgnoreRatio );
		msg.attribute.nPhysicalDefIgnore		-= static_cast< int >( attr.fPhysicalDefIgnore - attrByState.fPhysicalDefIgnore );
		msg.attribute.nPhysicalDefIgnoreRatio	-= static_cast< int >( attr.fPhysicalDefIgnoreRatio - attrByState.fPhysicalDefIgnoreRatio );
#ifndef USE_CLIENT97
		msg.attribute.nMagicalPenetration		-= static_cast< int >( attr.fMagicalPenetration - attrByState.fMagicalPenetration );
		msg.attribute.nMagicalPenetrationRatio	-= static_cast< int >( attr.fMagicalPenetrationRatio - attrByState.fMagicalPenetrationRatio );
		msg.attribute.nPhysicalPenetration		-= static_cast< int >( attr.fPhysicalPenetration - attrByState.fPhysicalPenetration );
		msg.attribute.nPhysicalPenetrationRatio	-= static_cast< int >( attr.fPhysicalPenetrationRatio - attrByState.fPhysicalPenetrationRatio );
#endif
//		msg.attribute.nAttackRange = 2000;
//		msg.attribute.nCoolTimeSpeed = 2300;				// 재시전속도	 
		msg.type		= TS_SC_STAT_INFO::BY_ITEM;
		PendMessage( pClient, &msg );
	}
}

void BroadcastPropertyMessage( unsigned rx, unsigned ry, unsigned char layer, AR_HANDLE handle, const char *szPropertyName, const __int64 & value )
{
	TS_SC_PROPERTY msg;
	s_strcpy( msg.name, _countof( msg.name ), szPropertyName );
	// 모든 패킷은 ZeroMemory가 되므로 name의 마지막 바이트는 0이 되어야 한다.
	// 아니면 szPropertyName이 너무 길어서 버퍼 오버런의 위험이 있다는 이야기
	// 여기에서 걸리면 TS_SC_PROPERTY::name의 크기 조절할 것.	
	msg.handle = handle;
	msg.value = value;
	msg.is_number = true;

	ArcadiaServer::Instance().Broadcast( rx, ry, layer, &msg );
}

void SendPropertyMessage( const struct StructPlayer *pPlayer, AR_HANDLE handle, const char *szPropertyName, const __int64 & value )
{
	TS_SC_PROPERTY msg;
	s_strcpy( msg.name, _countof( msg.name ), szPropertyName );
	// 모든 패킷은 ZeroMemory가 되므로 name의 마지막 바이트는 0이 되어야 한다.
	// 아니면 szPropertyName이 너무 길어서 버퍼 오버런의 위험이 있다는 이야기
	// 여기에서 걸리면 TS_SC_PROPERTY::name의 크기 조절할 것.	
	msg.handle = handle;
	msg.value = value;
	msg.is_number = true;
	PendMessage( pPlayer, &msg );
}

void SendPropertyMessage( const struct StructPlayer *pPlayer, AR_HANDLE handle, const char *szPropertyName, const char *szValue )
{
	TS_SC_PROPERTY msg;
	msg.size += static_cast< unsigned >( strlen(szValue)+1 );
	s_strcpy( msg.name, _countof( msg.name ), szPropertyName );
	// 모든 패킷은 ZeroMemory가 되므로 name의 마지막 바이트는 0이 되어야 한다.
	// 아니면 szPropertyName이 너무 길어서 버퍼 오버런의 위험이 있다는 이야기
	// 여기에서 걸리면 TS_SC_PROPERTY::name의 크기 조절할 것.	
	msg.handle = handle;
	msg.is_number = false;
	char szTmp[4096 + sizeof(msg)];
	s_memcpy( szTmp, sizeof( szTmp ), &msg, sizeof(msg) );
	s_memcpy( szTmp+sizeof(msg), sizeof( szTmp ) - sizeof( msg ), szValue, static_cast< unsigned >( strlen(szValue)+1 ) );
	PendStream( pPlayer, szTmp, msg.size );
}

void SendSPMsg( const StructPlayer *pPlayer, struct StructSummon *pSummon )
{
	TS_SC_SP msg;
	msg.sp = pSummon->GetSP();
	msg.max_sp = pSummon->GetMaxSP();
	msg.handle = pSummon->GetHandle();
	PendMessage( pPlayer, &msg );
}


void SendExpMsg( const StructPlayer *pPlayer, const StructCreature *pCreature )
{
	TS_SC_EXP_UPDATE msg;
	msg.handle = pCreature->GetHandle();
	msg.jp  = pCreature->GetJobPoint();
	msg.exp = pCreature->GetEXP();
	PendMessage( pPlayer, &msg );
}

void BroadcastLevelMsg( const struct StructCreature *pCreature )
{
	TS_SC_LEVEL_UPDATE msg;
	msg.handle = pCreature->GetHandle();
	msg.job_level = pCreature->GetJobLevel();
	msg.level = pCreature->GetLevel();

	ArcadiaServer::Instance().Broadcast( pCreature->GetRX(), pCreature->GetRY(), pCreature->GetLayer(), &msg );
}

void SendLevelMsg( const struct StructPlayer *pPlayer, const StructCreature *pCreature )
{
	TS_SC_LEVEL_UPDATE msg;
	msg.handle = pCreature->GetHandle();
	msg.job_level = pCreature->GetJobLevel();
	msg.level = pCreature->GetLevel();
	PendMessage( pPlayer, &msg );
}

void SendGoldChaosUpdateMsg( const StructPlayer *pPlayer )
{
	TS_SC_GOLD_UPDATE msg;
	msg.gold = pPlayer->GetGold().GetRawData();
	msg.chaos = pPlayer->GetChaos();
	PendMessage( pPlayer, &msg );
}

void SendMixResult( struct StructPlayer * pClient, AR_HANDLE *pHandle, unsigned count, unsigned int type )
{
	TS_SC_MIX_RESULT msg;
	msg.count = count;
	msg.size += static_cast< unsigned int >( sizeof(AR_HANDLE)*count );
	msg.type = type;		// 특수 조합 결과 (각성, 랜덤화 등)

	char szTmp[4096];
	s_memcpy( szTmp, sizeof( szTmp ), &msg, sizeof(msg) );
	if( pHandle && count )
		s_memcpy( szTmp+sizeof(msg), sizeof( szTmp ) - sizeof( msg ), pHandle, sizeof(AR_HANDLE)*count );
	PendStream( pClient, szTmp, msg.size );
}

void SendDecomposeResult( struct StructPlayer * pClient, TS_SC_DECOMPOSE_RESULT::DECOMPOSE_INFO * pItemResults, unsigned int count )
{
	TS_SC_DECOMPOSE_RESULT msg;

	msg.total_count = count;
	msg.size += sizeof( TS_SC_DECOMPOSE_RESULT::DECOMPOSE_INFO ) * count;
	
	char szTmp[4096];
	s_memcpy( szTmp, sizeof( szTmp ), &msg, sizeof( msg ) );
	s_memcpy( szTmp+sizeof( msg ), sizeof( szTmp ) - sizeof( msg ), pItemResults, sizeof( TS_SC_DECOMPOSE_RESULT::DECOMPOSE_INFO ) * count );
	PendStream( pClient, szTmp, msg.size );
}

void SendItemList( struct StructPlayer * pClient, bool bIsStorage )
{
	if( !pClient->GetItemCount() ) return;

	size_t cnt = 0;
	
	if( bIsStorage )	cnt = pClient->GetStorageItemCount();
	else				cnt = pClient->GetItemCount();

	TS_SC_INVENTORY msg;

	const static size_t MAX_ITEM_COUNT_PER_MSG = ( SENDMSG_BUFFER_SIZE - sizeof( msg ) ) / sizeof( TS_ITEM_INFO );
	// TS_SC_INVENTORY::count가 unsigned short 이므로 한 번에 0xFFFF 개까지만 송신 가능. 버퍼가 부족한 경우 체크
	assert( MAX_ITEM_COUNT_PER_MSG <= 0xFFFF );

	for( size_t idx = 0 ; idx < cnt ; idx += MAX_ITEM_COUNT_PER_MSG )
	{
		// 한 번에 MAX_ITEM_COUNT_PER_MSG 개수만큼씩만 묶어서 보냄
		msg.count = static_cast< unsigned short >( ( ( cnt - idx ) > MAX_ITEM_COUNT_PER_MSG ) ? MAX_ITEM_COUNT_PER_MSG : ( cnt - idx ) );
		// 아이템 정보만큼 크기를 늘림
		msg.size = static_cast< unsigned int >( sizeof( TS_SC_INVENTORY ) + sizeof( TS_ITEM_INFO ) * msg.count );

		char szBuffer[ SENDMSG_BUFFER_SIZE ] = "";

		// 버퍼에 헤더 복사
		s_memcpy( szBuffer, sizeof( szBuffer ), &msg, sizeof(msg) );
		
		// 버퍼에 개개의 아이템 정보 복사
		TS_ITEM_INFO * pInfo = reinterpret_cast< TS_ITEM_INFO * >( szBuffer + sizeof( msg ) );
		size_t nMaxItemIndex = idx + msg.count;
		for( size_t i = idx ; i < nMaxItemIndex ; ++i )
		{
			StructItem * pItem = bIsStorage ? pClient->GetStorageItem( static_cast< unsigned int >( i ) ) : pClient->GetItem( static_cast< unsigned int >( i ) );
			fillItemInfo( pInfo, pItem );				
			++pInfo;
		}

		// 메세지 전송
		PendStream( pClient, szBuffer, msg.size );
	}
}

void BroadcastSummonEvolutionMessage( struct StructPlayer * pClient, struct StructSummon *pSummon )
{
}


void SendAddSummonMessage( struct StructPlayer * pClient, struct StructSummon *pSummon )
{
	TS_SC_ADD_SUMMON_INFO msg;
	
	msg.summon_handle = pSummon->GetHandle();			// 소환수 핸들
	msg.card_handle = pSummon->GetParentCard()->GetHandle();
	msg.level = pSummon->GetLevel();
	s_strcpy( msg.name, _countof( msg.name ), pSummon->GetName() );	// 소환수 이름
	msg.code = pSummon->GetSummonCode();
	int nEffectID = pSummon->GetParentCard()->GetEffectID();

	msg.sp = pSummon->GetSP();// = nEffectID > 1000 ? nEffectID - 1000 : 0;
	PendMessage( pClient, &msg );

	SendStatInfo( pClient, pSummon );	// 소환수 스탯정보 얻어서
										// 보내주자

	SendHPMPMsg( pClient, pSummon, pSummon->GetHP(), pSummon->GetMP() );	// HP/MP
	SendLevelMsg( pClient, pSummon );	// 레벨
	SendExpMsg( pClient, pSummon );		// 경치

	SendSPMsg( pClient, pSummon );
}

void SendRemoveSummonMessage( struct StructPlayer * pClient, struct StructSummon *pSummon )
{
	TS_SC_REMOVE_SUMMON_INFO msg;
	msg.card_handle = pSummon->GetParentCard()->GetHandle();

	PendMessage( pClient, &msg );
}

void SendAddPetMessage( struct StructPlayer * pClient, struct StructPet *pPet )
{
	TS_SC_ADD_PET_INFO msg;
	msg.cage_handle = ( pPet->GetParentCage() ) ? pPet->GetParentCage()->GetHandle() : 0;
	msg.pet_handle = pPet->GetHandle();
	s_strcpy( msg.name, _countof( msg.name ), pPet->GetName() );
	msg.code = pPet->GetPetCode();

	PendMessage( pClient, &msg );
	//::SendPetSkillMessage(pClient, pPet, 46016);
}

void SendRemovePetMessage( struct StructPlayer * pClient, struct StructPet *pPet )
{
	TS_SC_REMOVE_PET_INFO msg;

	msg.handle = pPet->GetHandle();

	PendMessage( pClient, &msg );
}

void GetWearMsg( struct StructCreature *pCreature, TS_WEAR_INFO & itemInfoMsg )
{
	memset( &(itemInfoMsg.itemCode), 0, sizeof( itemInfoMsg.itemCode ) );
	memset( &(itemInfoMsg.itemEnhance), 0, sizeof( itemInfoMsg.itemEnhance ) );

	StructItem * pItem;
	itemInfoMsg.handle = pCreature->GetHandle();
	for( int i = 0; i < ItemBase::MAX_SPARE_ITEM_WEAR; ++i )
	{
		pItem = pCreature->GetWearedItem( (ItemBase::ItemWearType) i );
		if( !pItem ) continue;
		itemInfoMsg.itemCode[i] = pItem->GetItemCode();
		itemInfoMsg.itemEnhance[i] = pItem->GetItemEnhance();
		itemInfoMsg.itemLevel[i] = pItem->GetItemLevel();
		itemInfoMsg.elemental_effect_type[i] = pItem->GetElementalEffectType();
		itemInfoMsg.appearance_code[i] = pItem->GetAppearanceCode();
	}

	if( pCreature->IsPlayer() )
	{
		StructPlayer * pPlayer = static_cast< StructPlayer * >(pCreature);
		// 매직 넘버 -_-
		// if( !itemInfoMsg.itemCode[ ItemBase::WEAR_HELM ] ) itemInfoMsg.itemCode[ ItemBase::WEAR_HELM ] = pPlayer->GetBaseModelId( 0 );
		if( !itemInfoMsg.itemCode[ ItemBase::WEAR_ARMOR ] ) itemInfoMsg.itemCode[ ItemBase::WEAR_ARMOR ] = pPlayer->GetBaseModelId( 2 );
		if( !itemInfoMsg.itemCode[ ItemBase::WEAR_GLOVE ] ) itemInfoMsg.itemCode[ ItemBase::WEAR_GLOVE ] = pPlayer->GetBaseModelId( 3 );
		if( !itemInfoMsg.itemCode[ ItemBase::WEAR_BOOTS ] ) itemInfoMsg.itemCode[ ItemBase::WEAR_BOOTS ] = pPlayer->GetBaseModelId( 4 );
	}
}

void SendWearMessage( StructPlayer *pPlayer )
{
	SendWearMessage( pPlayer, pPlayer );
}

void SendWearMessage( struct StructPlayer *pPlayer, struct StructCreature *pCreature )
{
	TS_WEAR_INFO msg;
	GetWearMsg( pCreature, msg );
	PendMessage( pPlayer, &msg );
}

void SendTotalItemWearInfoMessage( StructPlayer *pPlayer, StructCreature *pTarget )
{
	if( !pPlayer || !pTarget )
		return;

	for( int i = 0; i < ItemBase::MAX_ITEM_WEAR; ++i )
	{
		StructItem * pItem = pTarget->GetWearedItem( (ItemBase::ItemWearType) i );
		if( !pItem ) continue;

		SendItemWearInfoMessage( pPlayer, pTarget, pItem );
	}
}


void SendItemWearInfoMessage( StructPlayer *pPlayer, StructCreature *pTarget, StructItem *pItem )
{
	TS_SC_ITEM_WEAR_INFO msg;

	msg.target_handle = pTarget ? pTarget->GetHandle() : NULL;
	msg.item_handle = pItem->GetHandle();
	msg.wear_position = pItem->GetWearInfo();
	msg.enhance = pItem->GetItemEnhance();
	msg.elemental_effect_type = pItem->GetElementalEffectType();
	msg.appearance_code = pItem->GetAppearanceCode();

	PendMessage( pPlayer, &msg );
}

void SendCharacterList( struct IStreamSocketConnection * pConn, struct LOBBY_CHARACTER_INFO *pInfo, unsigned short nCount, unsigned short nLastLoginIndex )
{
	if( nCount > 10 )
	{
		FILELOG( "SendCharacterList() : a lot of count : %d", nCount );
		_cprint( "SendCharacterList() : a lot of count : %d\n", nCount );
		return;
	}

	// 메세지 버퍼 할당
	char pBuf[SENDMSG_BUFFER_SIZE];
	// char *pBuf = new char[ sizeof( TS_SC_CHARACTER_LIST ) + sizeof( LOBBY_CHARACTER_INFO ) * nCount ];

	// 메세지 포인터 설정
	TS_SC_CHARACTER_LIST *pMsg = new (pBuf) TS_SC_CHARACTER_LIST;
	LOBBY_CHARACTER_INFO *pMsgInfo = reinterpret_cast< LOBBY_CHARACTER_INFO* >( pMsg+1 );
	int msg_buf_len = sizeof( pBuf ) - sizeof( TS_SC_CHARACTER_LIST );

	// 메인 메세지 초기화
	pMsg->size += static_cast< unsigned int >( sizeof( LOBBY_CHARACTER_INFO ) * nCount );

	if ( pMsg->size > SENDMSG_BUFFER_SIZE )
	{
		FILELOG( "SendCharacterList too big : %d", pMsg->size );
		_cprint( "SendCharacterList too big : %d", pMsg->size );
		return;
	}
	// pMsg->current_server_time = time( NULL );

	{
		extern XCriticalSection g_ConnectionTagLock;
		THREAD_SYNCHRONIZE( g_ConnectionTagLock );

		_CONNECTION_TAG * pTag = static_cast< _CONNECTION_TAG * >( pConn->GetTag() );

		pTag->vCharacterNameList.clear();
		// 캐릭터 정보 추가
		for( unsigned short i = 0; i < nCount ; ++i )
		{
			pTag->vCharacterNameList.push_back( pInfo->name );

			s_memcpy( pMsgInfo, msg_buf_len, pInfo, sizeof(LOBBY_CHARACTER_INFO) );
			msg_buf_len -= sizeof( LOBBY_CHARACTER_INFO );
			++pMsgInfo;
			++pInfo;
		}
	}

	pMsg->count = nCount;

	// 최근 선택 캐릭터 번호 추가
	pMsg->last_login_index = nLastLoginIndex;

	// 메세지 보내고
	PendMessage( pConn, pMsg );
}

void SendItemMessage( struct StructPlayer * pPlayer, struct StructItem *pItem )
{
	// 메세지 헤더
	char buf[ sizeof(TS_SC_INVENTORY)+sizeof(TS_ITEM_INFO) ];
	TS_SC_INVENTORY *pInvenMsg = new (buf) TS_SC_INVENTORY();

	// 아이템 정보만큼 크기를 늘린다
	pInvenMsg->size += sizeof(TS_ITEM_INFO);

	pInvenMsg->count = 1;
	
	// 버퍼에 개개의 아이템 정보 복사
	TS_ITEM_INFO * pIt = reinterpret_cast< TS_ITEM_INFO * >( buf + sizeof(TS_SC_INVENTORY) );
	fillItemInfo( pIt, pItem );

	// 메세지 전송
	PendMessage( pPlayer, pInvenMsg );
}


void SendCreatureEquipMessage( const StructPlayer *pPlayer, bool bShowDialog )
{
	TS_EQUIP_SUMMON msg;	

	for( int i = 0; i < 6; ++i )
	{
		StructItem *pCard = pPlayer->GetSummonCardAt( i );

		if( pCard ) msg.card_handle[i] = pCard->GetHandle();
		else msg.card_handle[i] = 0;
	}

	msg.open_dialog = bShowDialog;

	PendMessage( pPlayer, &msg );
}

void SendAccountResultWithAuth( struct IStreamSocketConnection * pConn, const char *szAccount, int nError )
{
	SendResult( pConn, TM_CS_ACCOUNT_WITH_AUTH, nError );
}

void SendDisconnectDesc( struct IStreamSocketConnection * pConn, const TS_SC_DISCONNECT_DESC::_DISCONNECT_TYPE eDisconnectionDesc )
{
	TS_SC_DISCONNECT_DESC msg;

	msg.desc_id = eDisconnectionDesc;

	PendMessage( pConn, &msg );
}

void SendLoginResult( struct IStreamSocketConnection * pConn, struct StructPlayer * pClient, const char nPCBangMode, const unsigned short nResult )
{
	// 접속이 더이상 유효하지 않다면
	/*
	if( !LockIOCPConnection( pConn ) )
	{	
		ArcadiaServer::Instance().DeleteObject( pClient );
		return;
	}*/

	TS_LOGIN_RESULT msg;

	msg.result = nResult;

	if( nResult == RESULT_SUCCESS )
	{
		// time sync
		SendTimeSync( pClient );

		msg.handle = pClient->GetHandle();
		msg.region_size = g_nRegionSize;
		msg.cell_size = CELL_SIZE;

		// 종족
		msg.race = pClient->GetRace();

		// 성별
		msg.sex = pClient->GetSex();

		// 외형 정보
		msg.skin_color = pClient->GetSkinColor();
		msg.hairId = pClient->GetBaseModelId( 0 );
		msg.faceId = pClient->GetBaseModelId( 1 );

		// 이름
		s_strcpy( msg.szName, _countof( msg.szName ), pClient->GetName() );

		msg.guild_id = pClient->GetGuildID();

		// 로그인 핸들러 호출
		ThreadPlayerHelper TPHelper( pClient );

		std::string strFirstLoginHandler;
		strFirstLoginHandler = "on_login( '";
		strFirstLoginHandler += pClient->GetName();
		strFirstLoginHandler += "' )";

		// on_login 에서 SCRIPT_SetValue 가 호출되며 프로퍼티 방송이 발생함
		{
			ARCADIA_LOCK( ArcadiaServer::Instance().LockObjectWithVisibleRange( pClient ) );
			LUA()->RunString( strFirstLoginHandler.c_str() );
		}

		// HP
		msg.hp = pClient->GetHP();
		msg.mp = pClient->GetMP();
		msg.max_hp = pClient->GetMaxHP();
		msg.max_mp = pClient->GetMaxMP();

		pClient->SetCurrentXY( pClient->GetX(), pClient->GetY() );

		// 위치 세팅
		msg.x = pClient->GetX();
		msg.y = pClient->GetY();
		msg.z = pClient->GetZ();
		msg.layer = pClient->GetLayer();
		msg.face_direction = pClient->GetFace();
	}

	pendMessage( pConn, &msg );

	// 로그인 성공이라면
	if( nResult == RESULT_SUCCESS )
	{
		pClient->SendCharacterInfo();

		// 퀵슬롯 정보는 캐쉬 아이템 서버에서는 프리패스 귀환권 지급 이후에 송신해야 함(DB_ReadCommercialStorageList)
		if( !GameRule::bIsCashUsableServer )
		{
			// Client Info (클라 인포, 퀵 슬롯, 저장 키 정보 등) 전송
			// !! 클라이언트 세팅 정보를 보낼 때에는 client_info를 항상 먼저 보내야 한다.
			SendPropertyMessage( pClient, pClient->GetHandle(), "client_info", pClient->GetClientInfo() );
			SendPropertyMessage( pClient, pClient->GetHandle(), "quick_slot", pClient->GetQuickSlot() );
			SendPropertyMessage( pClient, pClient->GetHandle(), "current_key", pClient->GetCurrentKey() );
			SendPropertyMessage( pClient, pClient->GetHandle(), "saved_key", pClient->GetSavedKey() );
		}

		if( pClient->GetPartyID() != 0 ) 
		{
			int nPartyID = pClient->GetPartyID();
			if( PartyManager::GetInstance().onLogin( pClient->GetPartyID(), pClient ) )
			{
				if( PartyManager::GetInstance().IsLinkedParty( nPartyID ) )
				{
					PrintfLinkedPartyChatMessage( false, CHAT_PARTY_SYSTEM, "@PARTY", pClient->GetPartyID(), "LOGIN|%s|%s|", PartyManager::GetInstance().GetPartyName( pClient->GetPartyID() ).c_str(), pClient->GetAlias() );

					BroadcastPartyMemberInfo( nPartyID, pClient );
					BroadcastLinkedPartyMemberInfo( nPartyID, pClient );

					SendPartyInfo( pClient );
					SendLinkedPartyInfo( pClient );
				}
				else
				{
					PrintfPartyChatMessage( CHAT_PARTY_SYSTEM, pClient->GetPartyID(), "LOGIN|%s|%s|", PartyManager::GetInstance().GetPartyName( pClient->GetPartyID() ).c_str(), pClient->GetName() );

					BroadcastPartyMemberInfo( nPartyID, pClient );

					SendPartyInfo( pClient );
				}

				BroadcastSummonInfo( nPartyID, pClient );
				SendSummonInfo( pClient );
			}
		}

		if( pClient->GetGuildID() != 0 )
		{
			PrintfGuildChatMessage( CHAT_GUILD_SYSTEM, pClient->GetGuildID(), "LOGIN|%s|%s|", GuildManager::GetInstance().GetGuildName( pClient->GetGuildID() ).c_str(), pClient->GetName() );
			BroadcastGuildMemberInfo( pClient->GetGuildID(), pClient );

			SendGuildStaticInfo( pClient );
			GuildManager::GetInstance().onLogin( pClient->GetGuildID(), pClient );
			SendGuildMemberInfo( pClient );
 		}

		SendQuestList( pClient );

		// 호칭 관련 처리
		SendTitleList( pClient );
		SendTitleConditionList( pClient );
		SendRemainTitleTime( pClient );
		SendMainTitle( pClient );
		SendSubTitle( pClient );

		// 피씨방 유저 세팅 및 지속효과 부여
		if( ENV().GetInt( "game.pcbang_bonus_server" ) )
		{
			ARCADIA_LOCK( ArcadiaServer::Instance().LockObjectWithVisibleRange( pClient ) );
			pClient->SetPCBangMode( nPCBangMode );
		}

		// 친구들처리
		{
			SendFriendsList( pClient );
			SendDenialsList( pClient );

			SendStatusMessageToFriendOfPlayer( pClient, true );
		}

		// 아이템 쿨타임 처리
		{
			// 클라 로딩과 쿨타임 처리 싱크 문제 발생으로 임시로 제거
			//SendItemCoolTimeInfo( pClient );
		}

		// 게임 중독 방지 시스템 관련 프로퍼티 송신
		{
			// SetContinuousPlayTime에서 스테미너 세이버 관련 지속효과 변화가 있을 경우 지역 방송 발생 함
			ARCADIA_LOCK( ArcadiaServer::Instance().LockObjectWithVisibleRange( pClient ) );

			AR_TIME nContinuousPlayTime = pClient->GetContinuousPlayTime();
			pClient->SetContinuousPlayTime( nContinuousPlayTime );	// 메시지 방송 및 스테미너 세이버 시간 정지 처리 수행
			SendPropertyMessage( pClient, pClient->GetHandle(), "playtime_limit1", GameRule::nMaxHealthyGameTime );
			SendPropertyMessage( pClient, pClient->GetHandle(), "playtime_limit2", GameRule::nMaxTiredGameTime );
		}

		// 기부 랭킹 보상 대상자일 경우 메시지 출력
		if( RankingManager::Instance().IsValidRankingScore( RankingManager::RANKING_TYPE_DONATION_REWARD, pClient->GetPlayerUID() ) )
		{
			SendChatMessage( false, CHAT_NOTICE, "@NOTICE", pClient, "@6748" );
		}
	}

	//UnLockIOCPConnection( pConn );

	if( GameRule::bUseLoginLogoutDebug )
	{
		_ctprint( "SendLoginResult (%s, %s, %hu)\n", pClient->GetAccountName(), pClient->GetName(), msg.result );
		FileLogHandler::GetFileLogHandler()->LogStringEx( NULL, "LoginLogout_Debug", "SendLoginResult (%s, %s, %hu)", pClient->GetAccountName(), pClient->GetName(), msg.result );
	}

	// nResult != RESULT_SUCCESS 일 때에는 SendLoginResult 함수 호출 측에서 LogoutNow 호출 해 줌.
	// 로그인 처리는 다 성공했는데 그 사이에 접속이 끊겨서 컨넥션이 없어진 경우에만 별도로 로그아웃 처리 해 줌
	if( nResult == RESULT_SUCCESS && pClient && !pClient->pConnection )
		pClient->LogoutNow( 3 );
}

void SendFriendsList( struct StructPlayer *pClient )
{
	std::string strResult = "FLIST|";

	struct FriendListEnumerator : public StructPlayer::FriendDenialFunctor
	{
		FriendListEnumerator( std::string & strFriendList, const char * pszName )
			: m_strFriendList( strFriendList )
			, m_pszName( pszName )
		{}

		virtual const bool operator()( std::string & strName )
		{
			m_strFriendList += strName;
			AR_HANDLE hTarget = StructPlayer::FindPlayer( strName.c_str() );
			StructPlayer::iterator itFriend = StructPlayer::get( hTarget );
			StructPlayer * pFriend = (*itFriend);

			if( pFriend )
			{
				std::vector< std::string >::const_iterator it = std::find( pFriend->m_vDenial.begin(), pFriend->m_vDenial.end(), m_pszName );

				if( it != pFriend->m_vDenial.end() )
				{	// 차단 됨(오프라인으로 보임)
					m_strFriendList += "|2|";
				}
				else
				{	// 온라인
					m_strFriendList += "|1|";
				}
			}
			else
			{	// 오프라인
				m_strFriendList += "|0|";
			}

			return true;
		}

	private:
		std::string &	m_strFriendList;
		const char *	m_pszName;
	} _FriendListEnumerator( strResult, pClient->GetName() );

	pClient->DoEachFriend( _FriendListEnumerator );

	SendChatMessage( false, CHAT_FRIEND_SYSTEM, "@FRIEND", pClient, strResult.c_str() );
}

void SendDenialsList( struct StructPlayer *pClient )
{
	std::string strResult = "DLIST|";

	struct DenialListEnumerator : public StructPlayer::FriendDenialFunctor
	{
		DenialListEnumerator( std::string & strDenialList )
			: m_strDenialList( strDenialList )
		{}

		virtual const bool operator()( std::string & strName )
		{
			m_strDenialList += strName;
			m_strDenialList += "|";

			return true;
		}

	private:
		std::string & m_strDenialList;
	} _DenialListEnumerator( strResult );

	pClient->DoEachDenial( _DenialListEnumerator );

	SendChatMessage( false, CHAT_FRIEND_SYSTEM, "@FRIEND", pClient, strResult.c_str() );
}

void SendStatusMessageToFriendOfPlayer( struct StructPlayer *pClient, struct StructPlayer *pTarget, bool bIsLogin )
{
	std::string strResult = "FSTATUS|";
	strResult += pClient->GetName();
	
	if( bIsLogin )
	{
		strResult += "|1|";
	}
	else
	{
		strResult += "|0|";
	}

	SendChatMessage( false, CHAT_FRIEND_SYSTEM, "@FRIEND", pTarget, strResult.c_str() );
}

void SendStatusMessageToFriendOfPlayer( struct StructPlayer *pClient, bool bIsLogin )
{
	std::string strResult = "FSTATUS|";
	strResult += pClient->GetName();
	
	if( bIsLogin )
	{
		strResult += "|1|";
	}
	else
	{
		strResult += "|0|";
	}

	struct FriendLoginNoticeFunctor : public StructPlayer::FriendDenialFunctor
	{
		FriendLoginNoticeFunctor( const StructPlayer * pClient, const std::string & strLoginNotice )
			: m_pClient( pClient )
			, m_strLoginNotice( strLoginNotice )
		{}

		virtual const bool operator()( std::string & strName )
		{
			AR_HANDLE hTarget = StructPlayer::FindPlayer( strName.c_str() );
			StructPlayer::iterator itFriend = StructPlayer::get( hTarget );
			StructPlayer * pFriend = (*itFriend);

			if( pFriend )
			{
				std::vector< std::string >::const_iterator it = std::find( m_pClient->m_vDenial.begin(), m_pClient->m_vDenial.end(), strName );
				if( it == m_pClient->m_vDenial.end() )
				{
					SendChatMessage( false, CHAT_FRIEND_SYSTEM, "@FRIEND", pFriend, m_strLoginNotice.c_str() );
				}
			}

			return true;
		}

	private:
		const StructPlayer *	m_pClient;
		const std::string &		m_strLoginNotice;
	} _FriendLoginNoticeFunctor( pClient, strResult );

	pClient->DoEachFriendOf( _FriendLoginNoticeFunctor );
}

void SendItemCoolTimeInfo( struct StructPlayer *pPlayer )
{
	TS_SC_ITEM_COOL_TIME msg;

	AR_TIME t = GetArTime();

	for( int i = 0; i < TS_SC_ITEM_COOL_TIME::MAX_ITEM_COOLTIME_GROUP; ++i )
	{
		msg.cool_time[i] = std::max( (int) pPlayer->GetItemCoolTime( i + 1 ) - (int) t, (int) 0 );
	}

	PendMessage( pPlayer, &msg );
}

void SendWindowMessage( struct StructPlayer *pPlayer, const char *szWindow, const char *szArgument, const char *szTrigger )
{
	TS_SC_SHOW_WINDOW msg;
	msg.window_length = static_cast<unsigned short>( strlen( szWindow ) );
	msg.argument_length = static_cast<unsigned short>( strlen( szArgument ) );
	msg.trigger_length = static_cast<unsigned short>( strlen( szTrigger ) );
	msg.size += ( msg.window_length + msg.argument_length + msg.trigger_length );

	if( msg.size > 1024 )
		return;

	char szTmp[ 1024 ];
	char *p = szTmp;
	int temp_size = sizeof( szTmp );

	s_memcpy( p, temp_size, &msg, sizeof(msg) );				p += sizeof(msg); temp_size -= sizeof(msg);
	s_memcpy( p, temp_size, szWindow, msg.window_length );		p += msg.window_length; temp_size -= msg.window_length;
	s_memcpy( p, temp_size, szArgument, msg.argument_length );	p += msg.argument_length; temp_size -= msg.argument_length;
	s_memcpy( p, temp_size, szTrigger, msg.trigger_length );	p += msg.trigger_length; temp_size -= msg.trigger_length;

	PendStream( pPlayer, szTmp, msg.size );
}

void SendDialogMessage( struct StructPlayer *pPlayer, AR_HANDLE npc_handle, int type, const char *szTitle, const char *szText, const char *szMenu )
{	
	TS_SC_DIALOG msg;
	msg.npc_handle = npc_handle;
	msg.type = type;
	msg.title_length = static_cast< unsigned short >( strlen(szTitle) );
	msg.text_length = static_cast< unsigned short >( strlen(szText) );
	msg.menu_length = static_cast< unsigned short >( strlen(szMenu) );
	msg.size += ( msg.title_length + msg.text_length + msg.menu_length );
	
	char szTmp[ 10*1024 ];
	char* pBuff = szTmp;
	int temp_size = sizeof( szTmp );
	if( msg.size > sizeof( szTmp ) )
	{
		pBuff = new char[msg.size];
		temp_size = msg.size;
	}

	char *p = pBuff;

	s_memcpy( p, temp_size, &msg, sizeof(msg) );			p += sizeof(msg); temp_size -= sizeof(msg);
	s_memcpy( p, temp_size, szTitle, msg.title_length );	p += msg.title_length; temp_size -= msg.title_length;
	s_memcpy( p, temp_size, szText, msg.text_length );		p += msg.text_length; temp_size -= msg.text_length;
	s_memcpy( p, temp_size, szMenu, msg.menu_length );

	PendStream( pPlayer, pBuff, msg.size );

	if( pBuff != szTmp )
	{
		delete [] pBuff;
	}
}

void SendShowCreateGuild( struct StructPlayer *pPlayer )
{
	if( !pPlayer )
		return;

	ARCADIA_LOCK( ArcadiaServer::Instance().LockObjectWithVisibleRange( pPlayer ) );

	TS_SC_SHOW_CREATE_GUILD msg;

	pPlayer->SetLastContact( "guild", 1 );

	PendMessage( pPlayer, &msg );
}

void SendShowCreateAlliance( struct StructPlayer *pPlayer )
{
	if( !pPlayer )
		return;

	ARCADIA_LOCK( ArcadiaServer::Instance().LockObjectWithVisibleRange( pPlayer ) );

	TS_SC_SHOW_CREATE_ALLIANCE msg;

	pPlayer->SetLastContact( "alliance", 1 );

	PendMessage( pPlayer, &msg );
}

void SendMarketInfo( struct StructPlayer *pPlayer, AR_HANDLE npc_handle, struct _MARKET_INFO *pInfo )
{
	// 마지막 접선한 상점 이름 저장
	pPlayer->SetLastContactMarket( pInfo->strMarketName.c_str() );

	// 헤더
	TS_SC_MARKET msg;
	msg.npc_handle = npc_handle;
	msg.item_count = static_cast< unsigned short >( pInfo->vItemList.size() );
	msg.size += sizeof(_MARKET_INFO::_MARKET_TAG)*msg.item_count;

	char szTmp[10240];

	s_memcpy( szTmp, sizeof( szTmp ), &msg, sizeof(msg) );

	TS_SC_MARKET::ItemInfo * pItemInfo = reinterpret_cast< TS_SC_MARKET::ItemInfo * >( szTmp+sizeof(msg) );

	for( int i = 0; i < msg.item_count; ++i )
	{
		pItemInfo->code = pInfo->vItemList[i].code;
		pItemInfo->price = pInfo->vItemList[i].price.GetRawData();
		pItemInfo->huntaholic_point = pInfo->vItemList[i].huntaholic_point;
		pItemInfo->arena_point = pInfo->vItemList[i].arena_point;

		++pItemInfo;
	}

	PendStream( pPlayer, szTmp, msg.size );
}

void SendGeneralMessageBox( struct StructPlayer *pPlayer, const char *szText )
{
	TS_SC_GENERAL_MESSAGE_BOX msg;
	
	msg.text_length = static_cast<unsigned short>( strlen( szText ) );
	msg.size += msg.text_length;

	if( msg.size > 1024 )
		return;

	char szTmp[ 1024 ];
	char *p = szTmp;
	int temp_size = sizeof( szTmp );

	s_memcpy( p, temp_size, &msg, sizeof(msg) );		p += sizeof(msg); temp_size -= sizeof(msg);
	s_memcpy( p, temp_size, szText, msg.text_length );	p += msg.text_length; temp_size -= msg.text_length;

	PendStream( pPlayer, szTmp, msg.size );
}

void SendItemDestroyMessage( struct StructPlayer * pPlayer, struct StructItem *pItem )
{
	TS_SC_DESTROY_ITEM msg;
	msg.item_handle = pItem->GetHandle();
	PendMessage( pPlayer, &msg );
}

void SendItemCountMessage( struct StructPlayer * pPlayer, struct StructItem *pItem )
{
	TS_SC_UPDATE_ITEM_COUNT msg;
	msg.item_handle = pItem->GetHandle();
	msg.count = pItem->GetCount();
	PendMessage( pPlayer, &msg );
}

void SendSkillCardInfo( struct StructItem *pItem )
{
	GameObject *pPtr = GameObject::raw_get( pItem->GetOwnerHandle() );
	if( !pPtr ) return;
	StructPlayer *pPlayer = static_cast< StructPlayer * >( pPtr );

	TS_SC_SKILLCARD_INFO msg;
	msg.item_handle = pItem->GetHandle();
	msg.target_handle = pItem->GetBindedCreatureHandle();

	PendMessage( pPlayer, &msg );
}

void SendShowSoulStoneCraftWindow( struct StructPlayer * pPlayer )
{
	TS_SC_SHOW_SOULSTONE_CRAFT_WINDOW msg;

	pPlayer->SetLastContact( "SoulStoneCraft", 1 );

	PendMessage( pPlayer, &msg );
}

void SendShowSoulStoneRepairWindow( struct StructPlayer * pPlayer )
{
	TS_SC_SHOW_SOULSTONE_REPAIR_WINDOW msg;
	
	pPlayer->SetLastContact( "RepairSoulStone", 1 );

	PendMessage( pPlayer, &msg );
}

void GenerateStateMessage( TS_SC_STATE& msg, AR_HANDLE target_handle, const struct StructState * pState, bool bIsCancel )
{
	msg.handle = target_handle;
	msg.state_handle = pState->GetUID();
	msg.state_code = pState->GetCode();

	msg.state_value = pState->GetStateValue();

	//s_strcpy( msg.state_string_value, _countof( msg.state_string_value ), pState->GetStateStringValue() );

	if( !bIsCancel )
	{
		msg.state_level	= pState->GetLevel();

		if( pState->IsAura() )
		{
			msg.end_time	= (AR_TIME)(-1);
		}
		else
		{
			msg.end_time	= pState->GetEndTime();
		}

		msg.start_time	= pState->GetStartTime();
	
		if( msg.start_time && msg.end_time > 0 && msg.start_time > msg.end_time )
		{
			FILELOG( "start time or end time error!!" );
			_cprint( "start time or end time error!!\n" );
			assert( 0 );
		}
	}
	else
	{
		msg.state_level	= 0;
		msg.end_time	= 0;
		msg.start_time	= 0;
	}
}

void BroadcastStateMessage( struct StructCreature *pCreature, struct StructState *p, bool bIsCancel )
{
	TS_SC_STATE msg;

	GenerateStateMessage( msg, pCreature->GetHandle(), p, bIsCancel );		
	ArcadiaServer::Instance().Broadcast( pCreature->GetRX(), pCreature->GetRY(), pCreature->GetLayer(), &msg );
}

void SendStateMessage( const struct StructPlayer *pPlayer, AR_HANDLE target_handle, const struct StructState *p, bool bIsCancel )
{
	TS_SC_STATE msg;

	GenerateStateMessage( msg, target_handle, p, bIsCancel );
	PendMessage( pPlayer, &msg );
}

void SendSkillResetMessage( const struct StructPlayer *pPlayer, const struct StructCreature *pCreature )
{
	TS_SC_SKILL_LIST msg;		

	msg.count = 0;
	msg.modification_type = TS_SC_SKILL_LIST::REFRESH;
	msg.target = pCreature->GetHandle();

	PendMessage( pPlayer, &msg );
}

void SendPetSkillMessage(const struct StructPlayer* pPlayer, const struct StructCreature* pCreature, int skill_id, bool refresh)
{
	char buf[4096];

	TS_SC_SKILL_LIST msg;

	msg.count = 0;
	msg.modification_type = refresh ? TS_SC_SKILL_LIST::REFRESH : TS_SC_SKILL_LIST::UPDATE;
	msg.target = pCreature->GetHandle();

	TS_SC_SKILL_LIST::SkillInfo info;
	info.skill_id = skill_id;
	info.base_skill_level = 0;// pCreature->GetBaseSkillLevel(skill_id);
	info.current_skill_level = 0;// pCreature->GetCurrentSkillLevel(skill_id);
	info.total_cool_time = 1; pCreature->GetTotalCoolTime(skill_id);
	info.remain_cool_time = 0; pCreature->GetRemainCoolTime(skill_id);

	msg.count = 1;
	s_memcpy(buf + sizeof(msg), sizeof(buf) - sizeof(msg), &info, sizeof(info));

	msg.size += msg.count * sizeof(TS_SC_SKILL_LIST::SkillInfo);

	s_memcpy(buf, sizeof(buf), &msg, sizeof(msg));
	if (msg.count || msg.modification_type == TS_SC_SKILL_LIST::REFRESH)	PendStream(pPlayer, buf, msg.size);
}

void SendSkillMessage( const struct StructPlayer *pPlayer, const struct StructCreature *pCreature, int skill_id, bool refresh )
{
	char buf[4096];

	TS_SC_SKILL_LIST msg;		

	msg.count = 0;
	msg.modification_type = refresh ? TS_SC_SKILL_LIST::REFRESH : TS_SC_SKILL_LIST::UPDATE;
	msg.target = pCreature->GetHandle();

	if( skill_id < 0 )
	{
		struct mySkillFunctor : StructCreature::SKILL_FUNCTOR
		{
			mySkillFunctor( const StructPlayer *p, char *q, int size ) : pPlayer( p ), pBuf( q ), nBufSize( size ), cnt(0)  {}

			virtual void onSkill( int sid, int skill_id, int base_skill_level, int current_skill_level, AR_TIME total_cool_time, AR_TIME remain_cool_time )
			{
				TS_SC_SKILL_LIST::SkillInfo info;
				info.skill_id = skill_id;
				info.base_skill_level = base_skill_level;
				info.current_skill_level = current_skill_level;
				info.total_cool_time = total_cool_time;
				info.remain_cool_time = remain_cool_time;

				assert( nBufSize >= sizeof(info) );
				if( nBufSize >= sizeof(info) )
				{
					s_memcpy( pBuf, nBufSize, &info, sizeof(info) );
					pBuf += sizeof(info);
					nBufSize -= sizeof(info);
					++cnt;
				}
				else
				{
					_cprint( "SendSkillMessage(skill_id) Creature INSUFFICIENT_BUFFER count: %d", cnt );
					FILELOG( "SendSkillMessage(skill_id) Creature INSUFFICIENT_BUFFER count: %d", cnt );
				}
			}
			unsigned cnt;
			char * pBuf;
			int nBufSize;
			const StructPlayer *pPlayer;
		} mySkillFunctorInst( pPlayer, buf + sizeof( msg ), sizeof(buf)-sizeof(msg) );

		pCreature->EnumSkill( mySkillFunctorInst );	
		msg.count = mySkillFunctorInst.cnt;
	}
	else if( pCreature->GetSkill( skill_id )->GetSkillUID() > 0 )
	{
		TS_SC_SKILL_LIST::SkillInfo info;
		info.skill_id = skill_id;
		info.base_skill_level = pCreature->GetBaseSkillLevel( skill_id );
		info.current_skill_level = pCreature->GetCurrentSkillLevel( skill_id );
		info.total_cool_time = pCreature->GetTotalCoolTime( skill_id );
		info.remain_cool_time = pCreature->GetRemainCoolTime( skill_id );

		msg.count = 1;
		s_memcpy( buf + sizeof(msg), sizeof( buf ) - sizeof( msg ), &info, sizeof(info) );
	}

	msg.size += msg.count * sizeof( TS_SC_SKILL_LIST::SkillInfo );

	s_memcpy( buf, sizeof( buf ), &msg, sizeof( msg ) );
	if( msg.count || msg.modification_type == TS_SC_SKILL_LIST::REFRESH )	PendStream( pPlayer, buf, msg.size );
}

void SendSkillMessage( const struct StructPlayer *pPlayer, const struct StructCreature *pCreature, std::vector< int > vSkill )
{
	char buf[4096];

	TS_SC_SKILL_LIST msg;		

	msg.count = 0;
	msg.modification_type = TS_SC_SKILL_LIST::UPDATE;
	msg.target = pCreature->GetHandle();

	struct mySkillFunctor : StructCreature::SKILL_FUNCTOR
	{
		mySkillFunctor( std::vector< int > & s, char *q, int size ) : vSkill( s ), pBuf( q ), nBuffSize( size ), cnt(0)  {}

		virtual void onSkill( int sid, int skill_id, int base_skill_level, int current_skill_level, AR_TIME total_cool_time, AR_TIME remain_cool_time )
		{
			std::vector< int >::iterator it = std::find( vSkill.begin(), vSkill.end(), skill_id );
			if( it != vSkill.end() )
			{
				TS_SC_SKILL_LIST::SkillInfo info;
				info.skill_id = skill_id;
				info.base_skill_level = base_skill_level;
				info.current_skill_level = current_skill_level;
				info.total_cool_time = total_cool_time;
				info.remain_cool_time = remain_cool_time;

				assert( nBuffSize >= sizeof(info) );
				if( nBuffSize >= sizeof(info) )
				{
					s_memcpy( pBuf, nBuffSize, &info, sizeof(info) );
					pBuf += sizeof(info);
					nBuffSize -= sizeof(info);
					++cnt;
				}
				else
				{
					_cprint( "SendSkillMessage(vSkill) Creature INSUFFICIENT_BUFFER count: %d", cnt );
					FILELOG( "SendSkillMessage(vSkill) Creature INSUFFICIENT_BUFFER count: %d", cnt );
				}

				vSkill.erase( it );
			}
		}

		std::vector< int > & vSkill;
		char * pBuf;
		int nBuffSize;
		unsigned cnt;
	} mySkillFunctorInst( vSkill, buf + sizeof( msg ), sizeof(buf)-sizeof(msg) );

	pCreature->EnumSkill( mySkillFunctorInst );

	msg.count = mySkillFunctorInst.cnt;
	msg.size += msg.count * sizeof( TS_SC_SKILL_LIST::SkillInfo );

	s_memcpy( buf, sizeof( buf ), &msg, sizeof( msg ) );
	if( msg.count )	PendStream( pPlayer, buf, msg.size );
}

void SendAddedSkillMessage( const struct StructPlayer *pPlayer, const struct StructCreature *pCreature )
{
	char buf[4096];

	TS_SC_ADDED_SKILL_LIST msg;

	msg.count = 0;
	msg.target = pCreature->GetHandle();

	struct mySkillFunctor : StructCreature::ADDED_SKILL_FUNCTOR
	{
		mySkillFunctor( const StructPlayer *p, char *q, int size ) : pPlayer( p ), pBuf( q ), nBuffSize( size ), cnt( 0 ) { }

		virtual void onSkill( int skill_id, bool restricted_to_type, char added_skill_level )
		{
			TS_SC_ADDED_SKILL_LIST::AddedSkillInfo info;
			info.skill_id = skill_id;
			info.restricted_to_type = restricted_to_type;
			info.added_skill_level = added_skill_level;
			
			assert( nBuffSize >= sizeof(info) );
			if( nBuffSize >= sizeof( info ) )
			{
				s_memcpy( pBuf, nBuffSize, &info, sizeof( info ) );
				pBuf += sizeof( info );
				nBuffSize -= sizeof( info );
				cnt++;
			}
			else
			{
				_cprint( "SendAddedSkillMessage() Creature INSUFFICIENT_BUFFER count: %d", cnt );
				FILELOG( "SendAddedSkillMessage() Creature INSUFFICIENT_BUFFER count: %d", cnt );
			}
		}

		unsigned cnt;
		char * pBuf;
		int nBuffSize;
		const StructPlayer *pPlayer;
	} mySkillFunctorInst( pPlayer, buf + sizeof( msg ), sizeof( buf ) - sizeof( msg ) );

	pCreature->EnumAddedSkill( mySkillFunctorInst );
	msg.count = mySkillFunctorInst.cnt;

	msg.size += msg.count * sizeof( TS_SC_ADDED_SKILL_LIST::AddedSkillInfo );

	s_memcpy( buf, sizeof( buf ), &msg, sizeof( msg ) );
	PendStream( pPlayer, buf, msg.size );
}

void SendSkillLevelMessage( const struct StructPlayer *pPlayer, const struct StructCreature *pCreature )
{
	char buf[4096];

	TS_SC_SKILL_LEVEL_LIST msg;		

	msg.count = 0;
	
	struct mySkillFunctor : StructCreature::SKILL_FUNCTOR
	{
		mySkillFunctor( const StructPlayer *p, char *q, int size ) : pPlayer( p ), pBuf( q ), nBuffSize( size ), cnt(0)  {}

		virtual void onSkill( int sid, int skill_id, int base_skill_level, int current_skill_level, AR_TIME total_cool_time, AR_TIME remain_cool_time )
		{
			TS_SC_SKILL_LEVEL_LIST::SkillLevelInfo info;
			info.skill_id = skill_id;
			info.skill_level = base_skill_level;
			
			assert( nBuffSize >= sizeof(info) );
			if( nBuffSize >= sizeof(info) )
			{
				s_memcpy( pBuf, nBuffSize, &info, sizeof(info) );
				pBuf += sizeof(info);
				nBuffSize -= sizeof(info);
				++cnt;
			}
			else
			{
				_cprint( "SendSkillLevelMessage() Creature INSUFFICIENT_BUFFER count: %d", cnt );
				FILELOG( "SendSkillLevelMessage() Creature INSUFFICIENT_BUFFER count: %d", cnt );
			}
		}
		unsigned cnt;
		char * pBuf;
		int nBuffSize;
		const StructPlayer *pPlayer;
	} mySkillFunctorInst( pPlayer, buf + sizeof( msg ), sizeof( buf ) - sizeof( msg ) );

	pCreature->EnumSkill( mySkillFunctorInst );	
	msg.count = mySkillFunctorInst.cnt;
	msg.size += msg.count * sizeof( TS_SC_SKILL_LEVEL_LIST::SkillLevelInfo );

	s_memcpy( buf, sizeof( buf ), &msg, sizeof( msg ) );
	PendStream( pPlayer, buf, msg.size );
}

void BroadcastSkillEffectMessage( const struct StructCreature *pCaster, const struct StructCreature *pTarget, int skill_level, int skill_id, int var1, unsigned char var2, char var3 )
{
	TS_SC_SKILL_EFFECT msg;
	msg.target = pTarget->GetHandle();
	msg.caster = pCaster->GetHandle();
	msg.var1 = var1;
	msg.var2 = var2;
	msg.var3 = var3;
	msg.skill_level = skill_level;
	msg.skill_id = skill_id;

	ArcadiaServer::Instance().Broadcast( pCaster->GetRX(), pCaster->GetRY(), pCaster->GetLayer(), &msg );
}

void SendDetectRangeMessage( const struct StructPlayer *pPlayer, const struct StructCreature *pCreature )
{
	TS_SC_DETECT_RANGE_UPDATE msg;
	msg.handle = pCreature->GetHandle();
	msg.detect_range = pCreature->GetDetectHideRange();
	PendMessage( pPlayer, &msg );
}

void SendTradeCancelMessage( struct StructPlayer * pClient )
{
	TS_TRADE msg;
	msg.mode = TS_TRADE::CANCEL_TRADE;
	msg.target_player = pClient->GetTradeTarget();
	PendMessage( pClient, &msg );
}

void SendCantAttackMsg( const struct StructPlayer *pPlayer, AR_HANDLE attacker_handle, AR_HANDLE target_handle, int reason )
{
	TS_SC_CANT_ATTACK msg;
	msg.attacker_handle = attacker_handle;
	msg.target_handle = target_handle;
	msg.reason = reason;
	PendMessage( pPlayer, &msg );
}

void SendAuraMsessage( const struct StructPlayer *pPlayer, const unsigned short skillId, const bool status )
{
	TS_SC_AURA msg;

	msg.caster = pPlayer->GetHandle();
	msg.skill_id = skillId;

	msg.status = status;

	PendMessage( pPlayer, &msg );
}

void SendAuraMsessage( const struct StructCreature *pCreature, const unsigned short skillId, const bool status )
{
	if( pCreature->IsPlayer() )
	{
		SendAuraMsessage( static_cast< const StructPlayer * >( pCreature ), skillId, status );
	}
	else if( pCreature->IsSummon() && static_cast< const StructSummon * >( pCreature )->GetMaster() )
	{
		SendAuraMsessage( static_cast< const StructSummon * >( pCreature )->GetMaster(), skillId, status);
	}
}

// 아래 구현이 더 효율적이지 않을까 싶음
/*
const	unsigned short TM_SC_PARTY_MEMBER_INFO		= 518;

struct TS_SC_PARTY_MEMBER_INFO : TS_MESSAGE
{
	TS_SC_PARTY_MEMBER_INFO()		_INIT( TM_SC_PARTY_MEMBER_INFO )

	AR_HANDLE		handle;
	int				level;
	int				job_id;
	unsigned char	hp_percentage;
	unsigned char	mp_percentage;
	int				x;
	int				y;

	unsigned char	name_length;
	char			name[];
};

void SendPartyMemberInfo( struct StructPlayer* pPlayer, struct StructPlayer* pTarget )
{
	char buf[256];
	TS_SC_PARTY_MEMBER_INFO* msg = reinterpret_cast< char* >( buf );

	msg->handle = pPlayer->GetHandle();
	msg->level = pPlayer->GetLevel();
	msg->job_id = pPlayer->GetJobId();
	msg->hp_percentage = pPlayer->GetHPPercentage();
	msg->mp_percentage = pPlayer->GetMPPercentage();
	msg->x = pPlayer->GetX();
	msg->y = pPlayer->GetY();
	msg->name_length = strlen( pPlayer->GetName() );	
	s_memcpy( msg+1, sizeof( buf ) - sizeof( msg ), pPlayer->GetName(), msg->name_length );
	msg->size += msg->size + msg->name_length;

	PendMessage( pTarget, msg );
}

void BroadcastPartyMemberInfo( int nPartyID, struct StructPlayer *pPlayer )
{
	struct myAttackTeamFunctor : PartyManager::PartyFunctor
	{
		myAttackTeamFunctor( StructPlayer *pPlayer ) : pPlayer( pPlayer ) {}

		virtual bool operator()( AR_HANDLE handle )
		{
			StructPlayer *pMember = static_cast< StructPlayer * >( GameObject::raw_get( *it ) );

			if( !pMember && IsVisibleRegion( pPlayer->GetRX(),  pPlayer->GetRY(), pMember->GetRX(), pMember->GetRX() )
			{
				SendPartyMemberInfo( pPlayer, pMember );
				return true;
			}
			else
			{
				return false
			}			
		}
		StructPlayer *pPlayer;
	} _fo( pPlayer, szString, nChatType );

	PartyManager::GetInstance().DoEachLinkedPartyMember( nPartyID, _fo );
}
*/

void BroadcastPartyMemberInfo( int nPartyID, struct StructPlayer *pPlayer )
{
	char buf[256];

	s_sprintf( buf, _countof( buf ), "MINFO|%u|%s|%d|%d|%d|%d|%d|%d|%d|", pPlayer->GetHandle(), pPlayer->GetAlias(), pPlayer->GetLevel(), pPlayer->GetJobId(), pPlayer->GetHPPercentage(), pPlayer->GetMPPercentage(), (int)pPlayer->GetX(), (int)pPlayer->GetY(), 2 );

	SendPartyChatMessage( CHAT_PARTY_SYSTEM, "@PARTY", nPartyID, buf, (unsigned)strlen(buf) );
}

void BroadcastLinkedPartyMemberInfo( int nPartyID, struct StructPlayer *pPlayer )
{
	char buf[256];

	if( !PartyManager::GetInstance().IsLinkedParty( nPartyID ) )
		return;

	s_sprintf( buf, _countof( buf ), "RMINFO|%u|%s|%d|%d|", pPlayer->GetHandle(), pPlayer->GetAlias(), pPlayer->GetJobId(), pPlayer->GetHPPercentage() != 0 );

	SendLinkedPartyChatMessage( false, CHAT_PARTY_SYSTEM, "@PARTY", nPartyID, buf );
}

void SendLinkedPartyChatMessage( bool bLock, int nChatType, const char *szSender, int nPartyID, const char *szString )
{
	struct myPartyFunctor : PartyManager::PartyFunctor
	{
		myPartyFunctor( const char *s, const char *p, int t ) : szSender( s ), pString( p ), nChatType( t ) {}

		virtual bool operator()( AR_HANDLE handle )
		{
			SendChatMessage( false, nChatType, szSender, handle, pString );

			return true;
		}
		int nChatType;
		const char *szSender;
		const char *pString;
	} _fo( szSender, szString, nChatType );

	if( PartyManager::GetInstance().IsLinkedParty( nPartyID ) )
		PartyManager::GetInstance().DoEachLinkedPartyMember( nPartyID, _fo );
	else
		PartyManager::GetInstance().DoEachMember( nPartyID, _fo );
}

void SendAttackTeamChatMessageByGuildID( bool bLock, int nChatType, const char *szSender, int nGuildID, const char *szString )
{
	int nPartyID = PartyManager::GetInstance().GetAttackTeamLeadPartyIDByGuildID( nGuildID );

	if( !nPartyID )
		return;

	SendLinkedPartyChatMessage( bLock, nChatType, szSender, nPartyID, szString );
}

void PrintfLinkedPartyChatMessage( bool bLock, int nChatType, const char *szSender, int nPartyID, const char *szString, ... )
{
	char szBuf[2048];
	va_list va;
	va_start( va, szString );
	s_vsprintf( szBuf, _countof(szBuf), szString, va );
	va_end( va );

	SendLinkedPartyChatMessage( bLock, nChatType, szSender, nPartyID, szBuf );
}

void PrintfAttackTeamChatMessageByGuildID( bool bLock, int nChatType, const char *szSender, int nGuildID, const char *szString, ... )
{
	char szBuf[2048];
	va_list va;
	va_start( va, szString );
	s_vsprintf( szBuf, _countof(szBuf), szString, va );
	va_end( va );

	SendAttackTeamChatMessageByGuildID( bLock, nChatType, szSender, nGuildID, szBuf);
}

void BroadcastLinkedPartyInfo( int nPartyID )
{
	std::string strLinkedPartyInfo = PartyManager::GetInstance().GetLinkedPartyInfo( nPartyID );

	if( !strLinkedPartyInfo.empty() )
		SendLinkedPartyChatMessage( false, CHAT_RAID_SYSTEM, "@RAID", nPartyID, strLinkedPartyInfo.c_str() );

}

void SendLinkedPartyInfo( struct StructPlayer *pPlayer )
{
	int nPartyID = pPlayer->GetPartyID();
	if( !nPartyID )
		return;

	if( !PartyManager::GetInstance().IsLinkedParty( nPartyID ) )
		return;

	std::string strRaidPartyInfo = PartyManager::GetInstance().GetLinkedPartyInfo( nPartyID );

	SendChatMessage( false, CHAT_RAID_SYSTEM, "@RAID", pPlayer, strRaidPartyInfo.c_str() );
}

void BroadcastPartyLeave( struct StructPlayer *pPlayer, bool bByBattleArena )
{
	BroadcastPartyLeave( pPlayer->GetPartyID(), pPlayer->GetName(), bByBattleArena );
}

void BroadcastPartyLeave( int nPartyID, const char * pszName, bool bByBattleArena )
{
	PrintfLinkedPartyChatMessage( false, CHAT_PARTY_SYSTEM, "@PARTY", nPartyID, "LEAVE|%s|%d|", pszName, ( bByBattleArena ) ? 1 : 0 );
}

void BroadcastPartyDestroy( int nPartyID, bool bByBattleArena )
{
	// 시즈/레이드 공대 파티의 마스터 파티가 해산되는 경우에는 링크된 모든 파티의 파티원들에게 각각 자신의 파티가 해산되는 것을 방송
	if( PartyManager::GetInstance().IsAttackTeamParty( nPartyID ) && nPartyID == PartyManager::GetInstance().GetLinkedPartyLeadPartyID( nPartyID ) )
	{
		struct myPartyDestroyFunctor : PartyManager::PartyFunctor
		{
			myPartyDestroyFunctor( bool _bByBattleArena )
				: bByBattleArena( _bByBattleArena )
			{}

			virtual bool operator()( AR_HANDLE handle )
			{
				StructPlayer *pPlayer = static_cast< StructPlayer * >( GameObject::raw_get( handle ) );
				if( !pPlayer ) return true;

				PrintfChatMessage( false, CHAT_PARTY_SYSTEM, "@PARTY", pPlayer, "DESTROY|%s|%d|", PartyManager::GetInstance().GetPartyName( pPlayer->GetPartyID() ).c_str(), ( bByBattleArena ) ? 1 : 0 );

				return true;
			}

			bool bByBattleArena;
		} _fo( bByBattleArena );

		PartyManager::GetInstance().DoEachLinkedPartyMember( nPartyID, _fo );
	}
	// 그 외에는 링크된 파티원 전원(링크된 파티가 없는 단일 파티라면 자기네 파티원 전원)이 해산된 파티에 대한 해산 메시지만 받으면 됨
	else
	{
		PrintfLinkedPartyChatMessage( false, CHAT_PARTY_SYSTEM, "@PARTY", nPartyID, "DESTROY|%s|%d|", PartyManager::GetInstance().GetPartyName( nPartyID ).c_str(), ( bByBattleArena ) ? 1 : 0 );
	}
}

void SendPartyInfoByID( struct StructPlayer *pPlayer, const int nPartyID )
{
	std::string strPartyInfo;

	// 배틀 아레나 연습 경기용 파티의 경우 경기 시작과 함께 실제 경기용 파티로 변경되므로 클라에는 무조건 실제 경기용 파티 타입으로 방송
	PartyManager::_PARTY_TYPE ePartyType = PartyManager::GetInstance().GetPartyType( nPartyID );
	XStringUtil::Format( strPartyInfo, "PINFO|%d|%s|%s|%d|%d|%d|%d|", nPartyID, PartyManager::GetInstance().GetPartyName( nPartyID ).c_str(),
		PartyManager::GetInstance().GetLeaderDisplayName( nPartyID ).c_str(), PartyManager::GetInstance().GetShareMode( nPartyID ),
		PartyManager::GetInstance().GetMaxLevel( nPartyID ), PartyManager::GetInstance().GetMinLevel( nPartyID ),
		( ePartyType != PartyManager::TYPE_BATTLE_ARENA_EXERCISE_TEAM ) ? ePartyType : PartyManager::TYPE_BATTLE_ARENA_TEAM );

	struct myPartyMemberTagFunctor : public PartyManager::PartyMemberTagFunctor
	{
		myPartyMemberTagFunctor( std::string & _strPartyInfo )
			: strPartyInfo( _strPartyInfo )
		{}

		virtual bool operator()( PartyManager::PartyMemberTag * pMemberTag )
		{
			bool bIsProc = false;

			do
			{
				if( !pMemberTag->bIsOnline )
					break;

				StructPlayer::iterator pit = StructPlayer::get( StructPlayer::FindPlayer( pMemberTag->strName.c_str() ) );
				StructPlayer * pPlayer = *pit;
				if( !pPlayer )
					break;

				char buf[512];
				s_sprintf( buf, _countof( buf ), "%u|%s|%d|%d|%d|%d|%d|%d|%d|", pPlayer->GetHandle(), pMemberTag->strDisplayName.c_str(), pMemberTag->nLevel, pMemberTag->nJobId, pPlayer->GetHPPercentage(), pPlayer->GetMPPercentage(), (int)pPlayer->GetX(), (int)pPlayer->GetY(), 2 );
				strPartyInfo += buf;

				bIsProc = true;

			} while( false );

			if( !bIsProc )
			{
				char buf[512];
				s_sprintf( buf, _countof( buf ), "0|%s|%d|%d|0|0|0|0|0|", pMemberTag->strDisplayName.c_str(), pMemberTag->nLevel, pMemberTag->nJobId );
				strPartyInfo += buf;
			}

			return true;
		}

		std::string & strPartyInfo;
	} _fo( strPartyInfo );

	PartyManager::GetInstance().DoEachMemberTag( nPartyID, _fo );

	SendChatMessage( false, CHAT_PARTY_SYSTEM, "@PARTY", pPlayer, strPartyInfo.c_str() );
}

void SendPartyInfo( struct StructPlayer *pPlayer )
{
	int nPartyID = pPlayer->GetPartyID();
	if( !nPartyID )
		return;

	SendPartyInfoByID( pPlayer, nPartyID );
}

void SendPartyPosition( struct StructPlayer *pPlayer )
{
	int nPartyID = pPlayer->GetPartyID();
	if( !nPartyID )
		return;

	struct myPartyFunctor : PartyManager::PartyFunctor
	{
		virtual bool operator()( AR_HANDLE handle )
		{
			StructPlayer *pPlayer = static_cast< StructPlayer * >( GameObject::raw_get( handle ) );
			if( !pPlayer ) return true;

			char buf[512];
			s_sprintf( buf, _countof( buf ), "%u|%d|%d|", pPlayer->GetHandle(), (int)pPlayer->GetX(), (int)pPlayer->GetY() );
			strList += buf;

			return true;
		}
		std::string					strList;
	} _fo;

	if( PartyManager::GetInstance().IsLinkedParty( nPartyID ) )
	{
		PartyManager::GetInstance().DoEachLinkedPartyMember( nPartyID, _fo );

		PrintfLinkedPartyChatMessage( false, CHAT_PARTY_SYSTEM, "@PARTY", nPartyID, "PPOS|%s", _fo.strList.c_str() );
	}
	else
	{
		PartyManager::GetInstance().DoEachMember( nPartyID, _fo );

		PrintfChatMessage( false, CHAT_PARTY_SYSTEM, "@PARTY", pPlayer, "PPOS|%s", _fo.strList.c_str() );
	}
}

void BroadcastGuildMemberInfo( int nGuildID, StructPlayer *pPlayer )
{
	char buf[256];

	s_sprintf( buf, _countof( buf ), "MINFO|%u|%s|%d|%d|%d|%d|%d|%d|%d|%d|", pPlayer->GetHandle(), pPlayer->GetName(), pPlayer->GetLevel(), pPlayer->GetJobId(), pPlayer->GetGuildPermission(), pPlayer->GetHPPercentage(), pPlayer->GetMPPercentage(), (int)pPlayer->GetX(), (int)pPlayer->GetY(), 2 );
	SendGuildChatMessage( CHAT_GUILD_SYSTEM, "@GUILD", nGuildID, buf, (unsigned)strlen(buf) );

	s_sprintf( buf, _countof( buf ), "MEMO|%s|%s|", pPlayer->GetName(), GuildManager::GetInstance().GetMemberMemo( nGuildID, pPlayer->GetSID() ).c_str() );
	SendGuildChatMessage( CHAT_GUILD_SYSTEM, "@GUILD", nGuildID, buf, (unsigned)strlen(buf) );
}

void SendGuildStaticInfo( struct StructPlayer * pPlayer )
{
	int nGuildID = pPlayer->GetGuildID();
	if( !nGuildID )
		return;

	// 소유 중인 던전 ID 얻기(연합 관련 체크 적용)
	int nOwnDungeonID = 0;
	{
		int nAllianceID = GuildManager::GetInstance().GetAllianceID( nGuildID );
		int nRaidGuildID = nGuildID;
		if( nAllianceID )
			nRaidGuildID = GuildManager::GetInstance().GetAllianceLeaderGuildID( nAllianceID );

		nOwnDungeonID = GuildManager::GetInstance().GetRaidDungeonID( nRaidGuildID );

		// 레이드/시즈 진행 중이지만 주인은 아닌 경우에는 제외
		if( DungeonManager::Instance().GetOwnGuildID( nOwnDungeonID ) != nRaidGuildID )
			nOwnDungeonID = 0;
	}

	char buf[512];
#ifdef USE_CLIENT97
	GuildManager::GuildInfo* pGuildInfo = GuildManager::GetInstance().getGuildInfo(nGuildID);
	s_sprintf( buf, _countof( buf ), "GINFO|%d|%s|%s|%d|%s|%d|%d|%s|%d|%d",
				nGuildID,
				GuildManager::GetInstance().GetGuildName( nGuildID ).c_str(),
				StructPlayer::GetPlayerName( GuildManager::GetInstance().GetLeaderSID( nGuildID ) ),
				GuildManager::GetInstance().GetMemberCount( nGuildID ),
				GuildManager::GetInstance().GetGuildNotice( nGuildID ).c_str(),
				GuildManager::GetInstance().IsRecruitingGuild( nGuildID ),
				nOwnDungeonID,
				GuildManager::GetInstance().GetGuildURL( nGuildID ).c_str(),
				pGuildInfo->nGrade,
				pGuildInfo->nPoint);
#else
	s_sprintf(buf, _countof(buf), "GINFO|%d|%s|%s|%d|%s|%d|%d|%s",
		nGuildID,
		GuildManager::GetInstance().GetGuildName(nGuildID).c_str(),
		StructPlayer::GetPlayerName(GuildManager::GetInstance().GetLeaderSID(nGuildID)),
		GuildManager::GetInstance().GetMemberCount(nGuildID),
		GuildManager::GetInstance().GetGuildNotice(nGuildID).c_str(),
		GuildManager::GetInstance().IsRecruitingGuild(nGuildID),
		nOwnDungeonID,
		GuildManager::GetInstance().GetGuildURL(nGuildID).c_str());

#endif


	SendChatMessage( false, CHAT_GUILD_SYSTEM, "@GUILD", pPlayer, buf );

	SendGuildPermissionInfo( pPlayer );
	SendAllianceInfo( pPlayer );
	SendGuildAdvertiseInfo( pPlayer );

	// 길드의 던전 레이드/시즈 팁은 해당 UI를 열었을 때 클라이언트가 채팅 명령어를 서버로 전달하므로 그 때만 보내면 됨
	//SendGuildDungeonRaidSiegeTip( pPlayer );
}

void SendGuildMemberInfo( struct StructPlayer * pPlayer )
{
	int nGuildID = pPlayer->GetGuildID();
	if( !nGuildID )
		return;

	struct myGuildFunctor : GuildManager::GuildFunctor
	{
		myGuildFunctor( StructPlayer * pPlayer )
			: m_pPlayer( pPlayer )
		{}

		virtual bool operator()( AR_HANDLE handle )
		{
			StructPlayer * pPlayer = static_cast< StructPlayer * >( GameObject::raw_get( handle ) );
			if( !pPlayer ) return true;

			char buf[512];
#ifdef USE_CLIENT97
			s_sprintf(buf, _countof(buf), "GMEMBER|%u|%s|%d|%d|%d|%d|%d|%d|%d|%d|%d|%d|", pPlayer->GetHandle(), pPlayer->GetName(), pPlayer->GetLevel(), pPlayer->GetJobId(), pPlayer->GetGuildPermission(), pPlayer->GetHPPercentage(), pPlayer->GetMPPercentage(), (int)pPlayer->GetX(), (int)pPlayer->GetY(), 2, pPlayer->GetGuildPoint(), pPlayer->GetTotalGuildPoint());
#else
			s_sprintf(buf, _countof(buf), "GMEMBER|%u|%s|%d|%d|%d|%d|%d|%d|%d|%d|", pPlayer->GetHandle(), pPlayer->GetName(), pPlayer->GetLevel(), pPlayer->GetJobId(), pPlayer->GetGuildPermission(), pPlayer->GetHPPercentage(), pPlayer->GetMPPercentage(), (int)pPlayer->GetX(), (int)pPlayer->GetY(), 2);
#endif
			SendChatMessage( false, CHAT_GUILD_SYSTEM, "@GUILD", m_pPlayer, buf );

			return true;
		}

		StructPlayer * m_pPlayer;
	} _fo( pPlayer );

	GuildManager::GetInstance().DoEachMember( nGuildID, _fo );

	std::vector< GuildManager::GuildMemberTag > vList;
	GuildManager::GetInstance().GetOfflineMember( nGuildID, vList );

	char buf[512];
	for( std::vector< GuildManager::GuildMemberTag >::iterator it = vList.begin(); it != vList.end(); ++it )
	{
#ifdef USE_CLIENT97
		s_sprintf( buf, _countof( buf ), "GMEMBER|0|%s|%d|%d|%d|0|0|0|0|0|%d|%d|", StructPlayer::GetPlayerName( (*it).sid ), (*it).nLevel, (*it).nJobId, (*it).nPermission, (*it).nPoint, (*it).nTotalPoint );
#else
		s_sprintf(buf, _countof(buf), "GMEMBER|0|%s|%d|%d|%d|0|0|0|0|0|", StructPlayer::GetPlayerName((*it).sid), (*it).nLevel, (*it).nJobId, (*it).nPermission);
#endif
		SendChatMessage( false, CHAT_GUILD_SYSTEM, "@GUILD", pPlayer, buf );
	}

	struct myGuildMemoFunctor : GuildManager::GuildMemberTagFunctor
	{
		myGuildMemoFunctor( StructPlayer * pPlayer )
			: m_pPlayer( pPlayer )
		{}

		virtual bool operator()( GuildManager::GuildMemberTag * pMemberTag )
		{
			char szBuffer[256];
			s_sprintf( szBuffer, _countof( szBuffer ), "MEMO|%s|%s|", StructPlayer::GetPlayerName( pMemberTag->sid ), pMemberTag->strMemo.c_str() );
			SendChatMessage( false, CHAT_GUILD_SYSTEM, "@GUILD", m_pPlayer, szBuffer );

			return true;
		}

	private:
		StructPlayer *	m_pPlayer;
	} _foMemo( pPlayer );

	GuildManager::GetInstance().DoEachMemberTag( nGuildID, _foMemo );
}

void SendGuildInfo( StructPlayer *pPlayer )
{
	SendGuildStaticInfo( pPlayer );
	SendGuildMemberInfo( pPlayer );
}

void SendGuildPermissionInfo( struct StructPlayer *pPlayer )
{
	int nGuildID = pPlayer->GetGuildID();
	if( !nGuildID )
		return;

	char buf[512];

	s_sprintf( buf, _countof( buf ), "GPERMISSION|%s|%d|%s|%d|%s|%d|%s|%d|%s|%d|%s|%d|",
				// PERMISSION_MEMBER_LEAST = 1
				GuildManager::GetInstance().GetPermissionName( nGuildID, 1 ).c_str(), GuildManager::GetInstance().GetPermissionSet( nGuildID, 1 ),
				GuildManager::GetInstance().GetPermissionName( nGuildID, 2 ).c_str(), GuildManager::GetInstance().GetPermissionSet( nGuildID, 2 ),
				GuildManager::GetInstance().GetPermissionName( nGuildID, 3 ).c_str(), GuildManager::GetInstance().GetPermissionSet( nGuildID, 3 ),
				GuildManager::GetInstance().GetPermissionName( nGuildID, 4 ).c_str(), GuildManager::GetInstance().GetPermissionSet( nGuildID, 4 ),
				GuildManager::GetInstance().GetPermissionName( nGuildID, 5 ).c_str(), GuildManager::GetInstance().GetPermissionSet( nGuildID, 5 ),
				GuildManager::GetInstance().GetPermissionName( nGuildID, 6 ).c_str(), GuildManager::GetInstance().GetPermissionSet( nGuildID, 6 ) );
				// PERMISSION_MEMBER_MAX = 6

	SendChatMessage( false, CHAT_GUILD_SYSTEM, "@GUILD", pPlayer, buf );
}

void getAllianceInfo( const int nAllianceID, char * pszBuf, size_t buf_len )
{
	int nLength = s_sprintf( pszBuf, buf_len, "AINFO|%d|%s|%d|%d|", nAllianceID, GuildManager::GetInstance().GetAllianceName( nAllianceID ).c_str(),
		GuildManager::GetInstance().GetAllianceMemberGuildCount( nAllianceID ), GuildManager::GetInstance().GetAllianceLeaderGuildID( nAllianceID ) );

	if( nLength < 0 )
	{
		return;
	}

	struct _myAllianceMemberFunctor : GuildManager::GuildListFunctor
	{
		_myAllianceMemberFunctor( char * pszBuffer, size_t len ) : m_pszBuffer( pszBuffer ), m_bufLen( len ) {}

		virtual bool operator()( int nGuildID, const char * szGuildName, const char * szGuildLeaderName, int nGuildLeaderLevel, int nGuildMemberCount, int nRaidDungeonID )
		{
			int nLength = s_sprintf( m_pszBuffer, m_bufLen, "%d|%s|%s|%d|", nGuildID, szGuildName, szGuildLeaderName, nGuildMemberCount );
			assert( nLength > 0 );
			if( nLength > 0 && m_bufLen >= nLength )
			{
				m_pszBuffer += nLength;
				m_bufLen -= nLength;
			}

			return true;
		}

		char * m_pszBuffer;
		size_t m_bufLen;
	} _fo( pszBuf + nLength, buf_len - nLength );

	GuildManager::GetInstance().DoEachAllianceGuild( nAllianceID, _fo );
}

void SendAllianceInfo( struct StructPlayer *pPlayer )
{
	int nGuildID = pPlayer->GetGuildID();
	if( !nGuildID )
		return;

	int nAllianceID = GuildManager::GetInstance().GetAllianceID( nGuildID );
	if( !nAllianceID )
		return;

	char buf[ 1024 ];
	getAllianceInfo( nAllianceID, buf, _countof( buf ) );

	SendChatMessage( false, CHAT_GUILD_SYSTEM, "@GUILD", pPlayer, buf );
}

void BroadcastAllianceInfo( const int nAllianceID )
{
	char buf[ 1024 ];
	getAllianceInfo( nAllianceID, buf, _countof( buf ) );

	std::vector< int > & vGuildList = GuildManager::GetInstance().GetAllianceMemberID( nAllianceID );
	for( std::vector< int >::const_iterator it = vGuildList.begin() ; it != vGuildList.end() ; ++it )
	{
		SendGuildChatMessage( CHAT_GUILD_SYSTEM, "@GUILD", (*it), buf );
	}
}

void SendGuildAdvertiseInfo( struct StructPlayer *pPlayer )
{
	int nGuildID = pPlayer->GetGuildID();
	if( !nGuildID )
		return;

	char buf[512];

	s_sprintf( buf, _countof( buf ), "GADV|%d|%d|%s|", GuildManager::GetInstance().GetGuildAdvertiseType( nGuildID ),
		static_cast< int >( GuildManager::GetInstance().GetGuildAdvertiseEndTime( nGuildID ) - time( NULL ) ),
		GuildManager::GetInstance().GetGuildAdvertiseComment( nGuildID ).c_str() );

	SendChatMessage( false, CHAT_GUILD_SYSTEM, "@GUILD", pPlayer, buf );
}

void SendGuildDungeonRaidSiegeTip( struct StructPlayer *pPlayer )
{
	int nGuildID = pPlayer->GetGuildID();
	if( !nGuildID )
		return;

	int nAllianceID = GuildManager::GetInstance().GetAllianceID( nGuildID );
	int nRaidGuildID = nGuildID;
	if( nAllianceID )
		nRaidGuildID = GuildManager::GetInstance().GetAllianceLeaderGuildID( nAllianceID );

	int nDungeonID = GuildManager::GetInstance().GetRaidDungeonID( nRaidGuildID );

	char buf[512];

	// 레이드 신청 전에는 자신의 길드에 대한 정보만 송신
	if( !nDungeonID )
	{
		s_sprintf( buf, _countof( buf ), "GRAIDSIEGETIP|0|0| | |0|0|" );
	}
	else
	{
		int nType = 0;
		int nTipGuildID = 0;
		AR_TIME nTipRecord = 0;

		// 레이드 기간이면 레이드 1위 길드 정보 확인
		if( DungeonManager::Instance().IsDungeonRaidTime( nDungeonID ) )
		{
			nType = 1;
			DungeonManager::Instance().GetTopDungeonRaidRecord( nDungeonID, nTipGuildID, nTipRecord );
		}
		// 시즈 기간이면
		else
		{
			// 주인 길드면 공격자 길드 정보 확인
			int nOwnerGuildID = DungeonManager::Instance().GetOwnGuildID( nDungeonID );
			if( nGuildID == nOwnerGuildID )
			{
				nType = 2;
				nTipGuildID = DungeonManager::Instance().GetRaidGuildID( nDungeonID );
				nTipRecord = DungeonManager::Instance().GetDungeonRaidRecord( nDungeonID, nTipGuildID );
			}
			// 공격자 길드면 주인 길드 정보 확인
			else
			{
				nType = 3;
				nTipGuildID = nOwnerGuildID;
			}
		}

		// 팁에 해당하는 길드에 대한 추가 정보 획득 및 방송 내용 생성
		if( nTipGuildID )
		{
			int nTipAllianceID = GuildManager::GetInstance().GetAllianceID( nTipGuildID );

			// 팁 길드가 연합 소속인 경우
			if( nTipAllianceID )
			{
				s_sprintf( buf, _countof( buf ), "GRAIDSIEGETIP|%d|%d|%s(%s)|%s|%d|%d|",
					nType, nTipRecord, GuildManager::GetInstance().GetAllianceName( nTipAllianceID ).c_str(),
					GuildManager::GetInstance().GetGuildName( nTipGuildID ).c_str(),
					StructPlayer::GetPlayerName( GuildManager::GetInstance().GetLeaderSID( nTipGuildID ) ),
					GuildManager::GetInstance().GetAllianceMemberPlayerCount( nTipAllianceID ), nDungeonID );
			}
			else
			{
				s_sprintf( buf, _countof( buf ), "GRAIDSIEGETIP|%d|%d|%s|%s|%d|%d|",
					nType, nTipRecord, GuildManager::GetInstance().GetGuildName( nTipGuildID ).c_str(),
					StructPlayer::GetPlayerName( GuildManager::GetInstance().GetLeaderSID( nTipGuildID ) ),
					GuildManager::GetInstance().GetMemberCount( nTipGuildID ), nDungeonID );
			}
		}
		else
		{
			s_sprintf( buf, _countof( buf ), "GRAIDSIEGETIP|%d|%d| | |0|%d|", nType, nTipRecord, nDungeonID );
		}
	}

	SendChatMessage( FALSE, CHAT_GUILD_SYSTEM, "@GUILD", pPlayer, buf );
}

void SendSummonInfo( struct StructPlayer *pPlayer )
{
	int nPartyID = pPlayer->GetPartyID();
	if( !nPartyID )
		return;

	std::string strSummonInfo;

	if( PartyManager::GetInstance().IsLinkedParty( nPartyID ) )
	{
		strSummonInfo = PartyManager::GetInstance().GetLinkedPartySummonInfo( nPartyID );
	}
	else
	{
		strSummonInfo = PartyManager::GetInstance().GetSummonInfo( nPartyID );
	}	
	
	SendChatMessage( false, CHAT_PARTY_SYSTEM, "@PARTY", pPlayer, strSummonInfo.c_str() );
}

void BroadcastSummonInfo( int nPartyID, struct StructPlayer *pPlayer, bool bForce )
{
	if( !nPartyID )
		return;

	int nMainSummonHandle = pPlayer->GetMainSummon() ? pPlayer->GetMainSummon()->GetHandle() : 0;
	int nSubSummonHandle = pPlayer->GetSubSummon() ? pPlayer->GetSubSummon()->GetHandle() : 0;

	if( !bForce && !nMainSummonHandle && !nSubSummonHandle )
		return;

	PrintfLinkedPartyChatMessage( false, CHAT_PARTY_SYSTEM, "@PARTY", nPartyID, "MSINFO|%d|%d|%d|%d|", nPartyID, pPlayer->GetHandle(), nMainSummonHandle, nSubSummonHandle );
}

void SendBeltSlotInfo( struct StructPlayer *pPlayer )
{
	TS_SC_BELT_SLOT_INFO msg;
	
	for( int i = 0; i < 8; ++i )
	{
		StructItem * pItem = pPlayer->GetBeltSlotCardAt( i );
		msg.handle[i] = pItem ? pItem->GetHandle() : 0;
	}

	PendMessage( pPlayer, &msg );
}

void SendOpenStorageMessage( struct StructPlayer *pPlayer )
{
	TS_SC_OPEN_STORAGE msg;
	msg.maxStorageItemCount = GameRule::nMaxStorageItemCount;
	PendMessage( pPlayer, &msg );
}

void SendJobInfo( struct StructPlayer *pPlayer, struct StructCreature *pCreature )
{
	SendPropertyMessage( pPlayer, pCreature->GetHandle(), "job", pCreature->GetJobId() );
	SendPropertyMessage( pPlayer, pCreature->GetHandle(), "job_level", pCreature->GetJobLevel() );
	for( int i = 0; i < GameRule::MAX_JOB_DEPTH - 1; ++i )
	{
		char buf[56];
		s_sprintf( buf, _countof( buf ), "job_%d", i );
		SendPropertyMessage( pPlayer, pCreature->GetHandle(), buf, pCreature->GetPrevJobId( i ) );
		s_sprintf( buf, _countof( buf ), "jlv_%d", i );
		SendPropertyMessage( pPlayer, pCreature->GetHandle(), buf, pCreature->GetPrevJobLevel( i ) );
	}
}

void SendTargetMsg( const struct StructPlayer *pPlayer, AR_HANDLE target )
{
	TS_SC_TARGET msg;
	msg.target = target;
	PendMessage( pPlayer, &msg );
}

void SendLeaveMsg( const ArObject * pClient, const ArObject * pObj )
{
	GameObject *pPtr = GameObject::raw_get( pObj->GetHandle() );
	if( !pPtr )
	{
		return;
	}

	//if( pClient != pObj && pPtr->IsPlayer() && static_cast< StructPlayer * >(pPtr)->IsInvisible() )
	//{
	//	return;
	//}

	TS_LEAVE msg;
	msg.handle = pObj->GetHandle();

/*	GameObject *pPtr = GameObject::raw_get( pObj->GetHandle() );

	if( pPtr )
	{

		if( pPtr->IsSummon() )
		{
			const StructSummon * pSummon = static_cast< const StructSummon * >( pObj );

			if( pSummon->GetSummonFlag() && !pSummon->IsWarping() && pSummon->GetMaster() == static_cast< const StructPlayer * >( pClient ) )
				return;
		}
	}
*/
	PendMessage( pClient, &msg );
}

void SendEnterMsg( const ArObject * pClient, const ArObject * pObj )
{
	char buf[1024];
	assert( sizeof(TS_ENTER) < sizeof(buf) );
 	TS_ENTER *pMsg = new (buf) TS_ENTER();
	ArPosition pos = pObj->GetCurrentPosition( GetArTime() );
	pMsg->x = pos.x;
	pMsg->y = pos.y;
	pMsg->z = pos.z;
	//pMsg->x = pObj->GetX();
	//pMsg->y = pObj->GetY();
	//pMsg->z = pObj->GetZ();
	pMsg->layer = pObj->GetLayer();
	pMsg->handle = pObj->GetHandle();

	GameObject *pPtr = GameObject::raw_get( pObj->GetHandle() );
	if( !pPtr )
	{
		//assert( 0 );
		return;
	}
	StructCreature *pCreature = NULL;
	bool bIsNeedStateNotify = false;

	switch( pObj->GetObjectType() )
	{
		case ArObject::STATIC_OBJECT:	
		{
			pMsg->type = TS_ENTER::STATIC_OBJECT;
			if( pPtr->IsItem() ) 
			{
				StructItem *pItem = static_cast< StructItem * >( pPtr );
				pMsg->ObjType = TS_ENTER::GAME_ITEM;

				TS_ENTER::ItemInfo *pItemInfo = (TS_ENTER::ItemInfo*)(pMsg+1);
				pMsg->size += sizeof(TS_ENTER::ItemInfo);

				pItemInfo->code = pItem->GetItemCode();
				pItemInfo->cnt = pItem->GetCount();
				pItemInfo->pick_up_order.drop_time = pItem->GetDropTime();

				const StructItem::ITEM_PICKUP_ORDER & ItemPickupOrder = pItem->GetPickupOrder();
#ifdef USE_CLIENT97
				for( int i = 0; i < 3; i++ )
				{
					pItemInfo->pick_up_order.hPlayer[i] = ItemPickupOrder.hPlayer[i];
					pItemInfo->pick_up_order.nPartyID[i] = ItemPickupOrder.nPartyID[i];
				}
				//pItemInfo->pick_up_order.hPlayer[0] = ItemPickupOrder.hPlayer[0];// 0xcccccccc;
#else
				for (int i = 0; i < 3; i++)
				{
					pItemInfo->pick_up_order.hPlayer[i] = ItemPickupOrder.hPlayer[i];
					pItemInfo->pick_up_order.nPartyID[i] = ItemPickupOrder.nPartyID[i];
				}

#endif
			}
			else if( pPtr->IsSkillProp() )
			{
				StructSkillProp *pSkillProp = static_cast< StructSkillProp * >( pPtr );
				pMsg->ObjType = TS_ENTER::GAME_SKILL_PROP;

				TS_ENTER::SkillInfo *pSkillInfo = (TS_ENTER::SkillInfo*)(pMsg+1);
				pMsg->size += sizeof(TS_ENTER::SkillInfo);

				pSkillInfo->caster = pSkillProp->GetCasterHandle();
				pSkillInfo->skill_num = pSkillProp->GetSkillId();
				pSkillInfo->start_time = pSkillProp->GetStartTime();
			}
			else if( pPtr->IsFieldProp() )
			{
				StructFieldProp *pFieldProp = static_cast< StructFieldProp * >( pPtr );
				pMsg->ObjType = TS_ENTER::GAME_FIELD_PROP;

				TS_ENTER::FieldPropInfo *pPropInfo = (TS_ENTER::FieldPropInfo*)(pMsg+1);
				pMsg->size += sizeof(TS_ENTER::FieldPropInfo);

				pPropInfo->prop_id = pFieldProp->GetPropId();
				pPropInfo->fZOffset = pFieldProp->GetZOffset();
				pPropInfo->fRotateX = pFieldProp->GetRotateX();
				pPropInfo->fRotateY = pFieldProp->GetRotateY();
				pPropInfo->fRotateZ = pFieldProp->GetRotateZ();
				pPropInfo->fScaleX = pFieldProp->GetScaleX();
				pPropInfo->fScaleY = pFieldProp->GetScaleY();
				pPropInfo->fScaleZ = pFieldProp->GetScaleZ();
				pPropInfo->bLockHeight = pFieldProp->IsHeightLocked();
				pPropInfo->fLockHeight = pFieldProp->GetLockHeight();
			}
			break;
		}
		case ArObject::MOVABLE_OBJECT:
		{
			pMsg->type = TS_ENTER::NPC;

			if( pPtr->IsNPC() )
			{
				pMsg->ObjType = TS_ENTER::GAME_NPC;

				TS_ENTER::NPCInfo *pNPCInfo = (TS_ENTER::NPCInfo*)(pMsg+1);
				pMsg->size += sizeof(TS_ENTER::NPCInfo);

				pNPCInfo->face_direction = static_cast<StructNPC*>( pPtr )->GetFace();
				pNPCInfo->npc_id = static_cast<StructNPC*>( pPtr )->GetNPCID();
#ifndef USE_CLIENT97
				pNPCInfo->hp = 100;
				pNPCInfo->max_hp = 100;
				pNPCInfo->mp = 0;
				pNPCInfo->max_mp = 0;
				pNPCInfo->level = static_cast<StructNPC*>( pPtr )->GetLevel();
				pNPCInfo->skin_color = static_cast<StructNPC*>( pPtr )->GetSkinColor();
				pNPCInfo->is_first_enter = false;
				pNPCInfo->energy = static_cast<StructNPC*>( pPtr )->GetEnergyCount();
#endif
				pNPCInfo->status = GetStatusCode( static_cast<StructPlayer*>( pPtr ), static_cast< const StructPlayer* >( pClient ) );

				// _cprint( "NPC ENTER : %s <- %s\n", static_cast< const StructPlayer*>( pClient )->GetName(), static_cast<const StructNPC*>( pPtr )->GetName() );
			}
			if( pPtr->IsMonster() )
			{
				pMsg->ObjType = TS_ENTER::GAME_MOB;

				TS_ENTER::MonsterInfo *pMonsterInfo = (TS_ENTER::MonsterInfo*)(pMsg+1);
				pMsg->size += sizeof(TS_ENTER::MonsterInfo);

				pMonsterInfo->face_direction = static_cast<StructMonster*>( pPtr )->GetFace();
				pMonsterInfo->monster_id = bits_scramble< int, 3 >( static_cast<StructMonster*>( pPtr )->GetMonsterId() );
#ifndef USE_CLIENT97
				pMonsterInfo->race = static_cast<StructCreature*>( pPtr )->GetCreatureGroup();
#endif
				pMonsterInfo->hp = static_cast<StructCreature*>( pPtr )->GetHP();
				pMonsterInfo->max_hp = static_cast<StructCreature*>( pPtr )->GetMaxHP();
				pMonsterInfo->mp = static_cast<StructCreature*>( pPtr )->GetMP();
				pMonsterInfo->max_mp = static_cast<StructCreature*>( pPtr )->GetMaxMP();
				pMonsterInfo->level = static_cast<StructCreature*>( pPtr )->GetLevel();
#ifndef USE_CLIENT97
				pMonsterInfo->skin_color = static_cast<StructCreature*>(pPtr)->GetSkinColor();
				pMonsterInfo->is_first_enter = static_cast<StructCreature*>( pPtr )->IsFirstEnter();
				pMonsterInfo->energy = static_cast<StructCreature*>( pPtr )->GetEnergyCount();
#endif
				pMonsterInfo->is_tamed = !!static_cast<StructMonster*>( pPtr )->GetTamer();
				pMonsterInfo->status = GetStatusCode( static_cast<StructPlayer*>( pPtr ), static_cast< const StructPlayer* >( pClient ) );

				if( static_cast<StructCreature*>( pPtr )->IsNeedStateNotify() )
				{
					pCreature = static_cast<StructCreature*>( pPtr );
					bIsNeedStateNotify = true;
				}
			}
			else if( pPtr->IsSummon() )
			{
				pMsg->ObjType = TS_ENTER::GAME_SUMMON;

				TS_ENTER::SummonInfo *pSummonInfo = (TS_ENTER::SummonInfo*)(pMsg+1);
				pMsg->size += sizeof(TS_ENTER::SummonInfo);

				StructSummon *pSummon = static_cast< StructSummon * >( pPtr );
				if( !pSummon ) break;

				pSummonInfo->face_direction = pSummon->GetFace();
				pSummonInfo->summon_code = pSummon->GetSummonCode();
				pSummonInfo->master_handle = pSummon->GetMaster()->GetHandle();
				s_strcpy( pSummonInfo->szName, _countof( pSummonInfo->szName ), pSummon->GetName() );
#ifndef USE_CLIENT97
				pSummonInfo->race = pSummon->GetCreatureGroup();
#endif
				pSummonInfo->hp = pSummon->GetHP();
				pSummonInfo->max_hp = pSummon->GetMaxHP();
				pSummonInfo->mp = pSummon->GetMP();
				pSummonInfo->max_mp = pSummon->GetMaxMP();
				pSummonInfo->level = pSummon->GetLevel();
#ifndef USE_CLIENT97
				pSummonInfo->skin_color = pSummon->GetSkinColor();
				pSummonInfo->is_first_enter = pSummon->IsFirstEnter();
				pSummonInfo->energy = pSummon->GetEnergyCount();
#endif
				pSummonInfo->enhance = 0;
				if( pSummon->GetParentCard() )
					pSummonInfo->enhance = pSummon->GetParentCard()->GetItemEnhance();
				if( pSummon->IsNeedStateNotify() )
				{
					pCreature = static_cast<StructCreature*>( pSummon );
					bIsNeedStateNotify = true;
				}
			}
			else if( pPtr->IsPet() )
			{
				pMsg->ObjType = TS_ENTER::GAME_PET;

				TS_ENTER::PetInfo *pPetInfo = (TS_ENTER::PetInfo*)(pMsg+1);
				pMsg->size += sizeof(TS_ENTER::PetInfo);

				StructPet *pPet = static_cast< StructPet * >( pPtr );
				pPetInfo->face_direction = pPet->GetFace();
				pPetInfo->pet_code = pPet->GetPetCode();
				pPetInfo->master_handle = pPet->GetMaster()->GetHandle();
				s_strcpy( pPetInfo->szName, _countof( pPetInfo->szName ), pPet->GetName() );
#ifndef USE_CLIENT97
				pPetInfo->race = pPet->GetCreatureGroup();
				pPetInfo->hp = 100;			// 사용 안 함
				pPetInfo->max_hp = 100;		// 사용 안 함
				pPetInfo->mp = 100;			// 사용 안 함
				pPetInfo->max_mp = 100;		// 사용 안 함
#endif
				pPetInfo->level = 100;// pPet->GetCreatureGroup();		// 사용 안 함
#ifndef USE_CLIENT97
				pPetInfo->skin_color = 0;	// 사용 안 함
				pPetInfo->is_first_enter = pPet->IsFirstEnter();
				pPetInfo->energy = 0;		// 사용 안 함
#endif
				if( pPet->IsNeedStateNotify() )
				{
					pCreature = static_cast<StructCreature*>( pPet );
					bIsNeedStateNotify = true;
				}
			}
			else if( pPtr->IsRoamer() )
			{
				// StructRoamer 객체에 대한 엔터 메시지는 유저한테 보내지 않음 -_ -;
				return;
			}
			else
			{
				pMsg->ObjType = TS_ENTER::GAME_NPC;
			}
			
			break;
		}
		case ArObject::CLIENT_OBJECT:
		{
			pMsg->type = TS_ENTER::PLAYER;
			pMsg->ObjType = TS_ENTER::GAME_PLAYER;

			TS_ENTER::PlayerInfo * pPlayerInfo = (TS_ENTER::PlayerInfo*)(pMsg+1);
			pMsg->size += sizeof(TS_ENTER::PlayerInfo);

			StructPlayer * pPlayer = static_cast< StructPlayer * >( pPtr );
			
			pPlayerInfo->hp = pPlayer->GetHP();
			pPlayerInfo->max_hp = pPlayer->GetMaxHP();

			pPlayerInfo->mp = pPlayer->GetMP();
			pPlayerInfo->max_mp = pPlayer->GetMaxMP();
			pPlayerInfo->level = pPlayer->GetLevel();

			pPlayerInfo->race = pPlayer->GetRace();
			pPlayerInfo->skin_color = pPlayer->GetSkinColor();

			// 캐릭터의 이름 세팅(가명 기능에 따른 추가 처리 포함)
			{
				// 가명 사용 여부 세팅과는 별개로 아레나 경기장 내부에서만 엔터 메시지를 가명으로 보내야 함
				// * 보통 Enter 메시지 방송 후에 pPlayer->ChangeLocation 함수가 호출되기 때문에
				//   m_pWorldLocation 이나 m_nWorldLocationId 에 의존해서 아레나 경기장 내부인지 판별하면 안 됨
				//   이 지점에서 유효한 정보는 좌표밖에 없음...;;;

				bool bUseAlias = false;
				// 가장 부담되는 GetLocationId, GetLocationType의 호출을 가능한 피하기 위해
				// 가명 기능이 활성화돼있고 경기에 참여 중인 유저만 위치 체크 진행
				if( pPlayer->IsUsingAlias() && pPlayer->GetBattleArenaInstanceNo() )
				{
					int nLocationID = GameContent::GetLocationId( pos.x, pos.y );
					if( nLocationID && WorldLocationManager::Instance().GetLocationType( nLocationID ) == StructWorldLocation::LOCATION_BATTLE_ARENA )
					{
						s_strcpy( pPlayerInfo->szName, _countof( pPlayerInfo->szName ), pPlayer->GetAlias() );
						bUseAlias = true;
					}
				}
				if( !bUseAlias )
				{
					s_strcpy( pPlayerInfo->szName, _countof( pPlayerInfo->szName ), pPlayer->GetName() );
				}
			}

			pPlayerInfo->sex = pPlayer->GetSex();

			// 오토들 덕분에 생긴 찌꺼기 정보가 ObjType 0에 성별, 종족이 0으로 날아간다.
			// 이런 경우는 그냥 무시해주자. 실제로 플레이어라면 둘 중 어떤 것도 0은 될 수 없다.

			if( !pPlayerInfo->race || !pPlayerInfo->sex )
			{
				FILELOG("TS_ENTER: Invalid object info - Postion: (%d, %d, %d) / Handle: %d", pMsg->x, pMsg->y, pMsg->layer, pMsg->handle );
				return;
			}

			// 투명인 케릭터는 엔터 메시지를 안보내기
			//if( pObj != pClient && static_cast< StructCreature * >(pPtr)->IsInvisible() )
			//{
			//	return;
			//}

			pPlayerInfo->face_direction = pPlayer->GetFace();
#ifndef USE_CLIENT97
			pPlayerInfo->is_first_enter = pPlayer->IsFirstEnter();
#else


#endif
			pPlayerInfo->energy = pPlayer->GetEnergyCount();
			pPlayerInfo->faceId = pPlayer->GetBaseModelId( 1 );
			pPlayerInfo->faceTextureId = pPlayer->GetFaceTextureId();
			pPlayerInfo->hairId = pPlayer->GetBaseModelId( 0 );
			pPlayerInfo->hairColorIndex = pPlayer->GetHairColorIndex();
			pPlayerInfo->hairColorRGB = pPlayer->GetHairColorRGB();

			pPlayerInfo->hideEquipFlag = pPlayer->GetHideEquipFlag();

			pPlayerInfo->job_id	= pPlayer->GetJobId();
			pPlayerInfo->status = GetStatusCode( pPlayer, static_cast< const StructPlayer* >( pClient ) );
			pPlayerInfo->ride_handle = pPlayer->GetRideHandle();

			if( pPlayerInfo->ride_handle )
			{
				SendEnterMsg( pClient, pPlayer->GetRideObject() );
			}

			if( pPlayer->IsNeedStateNotify() )
			{
				pCreature = pPlayer;
				bIsNeedStateNotify = true;
			}

			pPlayerInfo->guild_id = pPlayer->GetGuildID();
			pPlayerInfo->title_code = pPlayer->GetMainTitle() ? pPlayer->GetMainTitle()->nID : 0;
			pPlayerInfo->unkp = 0;

			break;
		}
	}

	// 엔터 메세지 전송
	PendMessage( pClient, pMsg );
	
	// 이동중인 녀석이라면 이동정보 보내주자
	if( pObj->GetObjectType() != ArObject::STATIC_OBJECT && pObj->IsMoving() )
	{
		SendMoveMsg( pClient, static_cast< const ArObject* >( pObj ) );
	}

	// 플레이어라면 아이템 정보 보내주자
	if( pMsg->type == TS_ENTER::PLAYER )
	{
		TS_WEAR_INFO wearInfo;
		GetWearMsg( static_cast<StructPlayer*>( pPtr ), wearInfo );
		PendMessage( pClient, &wearInfo );
	}

	// 상태이상
	if( bIsNeedStateNotify && pCreature )
	{
		const std::vector< StructState > & vStateList = pCreature->GetStateList();

		for( std::vector< StructState >::const_iterator it = vStateList.begin(); it != vStateList.end(); ++it )
		{
			//if( (*it).IsNeedDisplay() )
			//{
				SendStateMessage( static_cast< const StructPlayer* >( pClient ), pCreature->GetHandle(), &(*it) );
			//}
		}
	}
}

void SendMoveMsg( const ArObject * pClient, const ArObject * pObj )
{
	size_t nSize = sizeof( TS_MOVE ) + sizeof( TS_MOVE::MOVE_INFO ) * pObj->GetTargetPosList().size();
	if ( nSize > SENDMSG_BUFFER_SIZE )
	{
		FILELOG( "SendMoveMsg too big : %d", nSize );
		_cprint( "SendMoveMsg too big : %d", nSize );
		return;
	}
	char buf[SENDMSG_BUFFER_SIZE];

	// char * buf = new char[ sizeof( TS_MOVE ) + sizeof( TS_MOVE::MOVE_INFO ) * pObj->GetTargetPosList().size() ];

	TS_MOVE * pMsg = new (buf) TS_MOVE();

	pMsg->handle = pObj->GetHandle();
	pMsg->start_time = pObj->lastStepTime;
	pMsg->speed = pObj->GetSpeed();
	pMsg->count = static_cast<unsigned short>( pObj->GetTargetPosList().size() );
	pMsg->size += sizeof( TS_MOVE::MOVE_INFO ) * pMsg->count;
	pMsg->tlayer = pObj->GetLayer();

	TS_MOVE::MOVE_INFO * pMoveInfo = reinterpret_cast< TS_MOVE::MOVE_INFO * >( pMsg + 1 );
	std::vector< ArMoveVector::MOVE_INFO >::const_iterator it;

	for( it = pObj->GetTargetPosList().begin(); it != pObj->GetTargetPosList().end(); ++it, ++pMoveInfo )
	{
		pMoveInfo->tx = (*it).end.x;
		pMoveInfo->ty = (*it).end.y;
	}

	PendStream( pClient, buf, pMsg->size );

	// delete [] buf;
}

void SendForceMoveMsg( const ArObject * pClient, const ArObject * pObj )
{
	TS_FORCE_MOVE msg;
	msg.handle = pObj->GetHandle();
	msg.x = pObj->GetX();
	msg.y = pObj->GetY();
	msg.z = pObj->GetZ();
	msg.layer = pObj->GetLayer();
	msg.face = pObj->GetFace();

	PendMessage( pClient, &msg );
}

void SendQuestList( struct StructPlayer *pPlayer )
{
	// 패킷 버퍼(TS_SC_QUEST_LIST 패킷의 예측 가능한 최대 크기 만큼 할당, TS_QUEST_INFO가 TS_PENDING_QUEST_INFO 보다 크다고 가정함)
	char szBuf[ SENDMSG_BUFFER_SIZE ];
	memset( szBuf, 0, sizeof( szBuf ) );

	TS_SC_QUEST_LIST * pMsg = new (szBuf) TS_SC_QUEST_LIST;

	// 시작되었던 퀘스트 정보 버퍼에 복사
	struct myActiveQuestFunctor : public StructQuestManager::QuestFunctor
	{
		myActiveQuestFunctor( TS_SC_QUEST_LIST::TS_QUEST_INFO * pQuestInfoBuffer ) : m_pQuestInfoBuffer( pQuestInfoBuffer ) {}

		virtual bool operator()( StructQuest * pQuest )
		{
			m_pQuestInfoBuffer->code = pQuest->GetQuestCode();
			m_pQuestInfoBuffer->nStartID = pQuest->GetQuestInstance().nStartID;
			m_pQuestInfoBuffer->nProgress = ( pQuest->IsFinishable() ) ? QuestInstance::FINISHABLE : pQuest->GetProgress();
			m_pQuestInfoBuffer->nTimeLimit = ( pQuest->GetTimeLimitType() != QuestBase::TIME_LIMIT_TYPE_PERMANENT ) ? pQuest->GetTimeLimit() : 0;

			s_memcpy( m_pQuestInfoBuffer->nStatus, sizeof( m_pQuestInfoBuffer->nStatus ), pQuest->GetQuestInstance().nStatus, sizeof( m_pQuestInfoBuffer->nStatus ) );

			if( pQuest->IsRandomQuest() )
			{
				m_pQuestInfoBuffer->nValue[0] = pQuest->GetRandomKey( 0 );
				m_pQuestInfoBuffer->nValue[2] = pQuest->GetRandomKey( 1 );
				m_pQuestInfoBuffer->nValue[4] = pQuest->GetRandomKey( 2 );
				m_pQuestInfoBuffer->nValue[1] = pQuest->GetRandomValue( 0 );
				m_pQuestInfoBuffer->nValue[3] = pQuest->GetRandomValue( 1 );
				m_pQuestInfoBuffer->nValue[5] = pQuest->GetRandomValue( 2 );
			}

			++m_pQuestInfoBuffer;

			return true;
		}

		TS_SC_QUEST_LIST::TS_QUEST_INFO * m_pQuestInfoBuffer;
	} _aqfo( reinterpret_cast< TS_SC_QUEST_LIST::TS_QUEST_INFO * >( pMsg + 1 ) );

	pMsg->count_active = static_cast< unsigned short >( pPlayer->DoEachActiveQuest( _aqfo ) );
	// 퀘스트 정보만큼 크기를 늘린다
	pMsg->size += static_cast< unsigned int >( sizeof( TS_SC_QUEST_LIST::TS_QUEST_INFO ) * pMsg->count_active );

	// 시작 대기 중인 퀘스트 정보 버퍼에 복사
	struct myPendingQuestFunctor : public StructQuestManager::QuestFunctor
	{
		myPendingQuestFunctor( TS_SC_QUEST_LIST::TS_PENDING_QUEST_INFO * pQuestInfoBuffer ) : m_pQuestInfoBuffer( pQuestInfoBuffer ) {}

		virtual bool operator()( struct StructQuest * pQuest )
		{
			m_pQuestInfoBuffer->code = pQuest->GetQuestCode();
			m_pQuestInfoBuffer->nStartID = pQuest->GetQuestInstance().nStartID;

			++m_pQuestInfoBuffer;

			return true;
		}

		TS_SC_QUEST_LIST::TS_PENDING_QUEST_INFO * m_pQuestInfoBuffer;
	} _pqfo( reinterpret_cast< TS_SC_QUEST_LIST::TS_PENDING_QUEST_INFO * >( _aqfo.m_pQuestInfoBuffer ) );

	pMsg->count_pending = static_cast< unsigned short >( pPlayer->DoEachPendingQuest( _pqfo ) );
	// 퀘스트 정보만큼 크기를 늘린다
	pMsg->size += static_cast< unsigned int >( sizeof( TS_SC_QUEST_LIST::TS_PENDING_QUEST_INFO ) * pMsg->count_pending );

	// 메세지 전송
	PendStream( pPlayer, pMsg, pMsg->size );
}

void SendQuestStatus( struct StructPlayer *pPlayer, struct StructQuest* pQuest )
{
	if( !pQuest )
		return;

	TS_SC_QUEST_STATUS msg;
	msg.code = pQuest->GetQuestCode();

	for( int i = 0; i < QuestInstance::MAX_STATUS; ++i )
	{
		msg.nStatus[i] = pQuest->GetStatus( i );
	}

	msg.nProgress = ( pQuest->IsFinishable() ) ? QuestInstance::FINISHABLE : pQuest->GetProgress();
	msg.nTimeLimit = ( pQuest->GetTimeLimitType() != QuestBase::TIME_LIMIT_TYPE_PERMANENT ) ? pQuest->GetTimeLimit() : 0;;

	PendMessage( pPlayer, &msg );
}

void SendTitleList( struct StructPlayer *pPlayer )
{
	char szBuf[ SENDMSG_BUFFER_SIZE ] = "";
	char *pBuf = szBuf;
	
	size_t nCount = pPlayer->GetTitleCount();
	if( SENDMSG_BUFFER_SIZE <= sizeof( TS_SC_TITLE_LIST ) + sizeof( TS_SC_TITLE_LIST::TS_TITLE_INFO ) * nCount )
		pBuf = new char[ sizeof( TS_SC_TITLE_LIST ) + sizeof( TS_SC_TITLE_LIST::TS_TITLE_INFO ) * nCount ];

	TS_SC_TITLE_LIST * pMsg = new (pBuf) TS_SC_TITLE_LIST;

	nCount = 0;
	struct myTitleFunctor : public StructTitleManager::TitleFunctor
	{
		myTitleFunctor( TS_SC_TITLE_LIST::TS_TITLE_INFO * pTitleInfoBuffer, size_t & nCount )
			: m_pTitleInfoBuffer( pTitleInfoBuffer )
			, m_nCount( nCount )
		{}

		virtual bool operator()( StructTitle * pTitle )
		{
			m_pTitleInfoBuffer->code = pTitle->GetCode();
			m_pTitleInfoBuffer->status = pTitle->GetStatus();

			++m_pTitleInfoBuffer;
			++m_nCount;

			return true;
		}

		TS_SC_TITLE_LIST::TS_TITLE_INFO * m_pTitleInfoBuffer;
		size_t & m_nCount;
	} _atfo( reinterpret_cast< TS_SC_TITLE_LIST::TS_TITLE_INFO * >( pMsg + 1 ), nCount );

	pPlayer->DoEachTitle( _atfo );
	pMsg->count = static_cast< unsigned short >( nCount );
	pMsg->size += static_cast< unsigned int >( sizeof( TS_SC_TITLE_LIST::TS_TITLE_INFO ) * pMsg->count );

	// 메세지 전송
	PendStream( pPlayer, pMsg, pMsg->size );

	if( szBuf != pBuf )
		delete[] pBuf;
}

void SendTitleConditionList( struct StructPlayer *pPlayer )
{
	char szBuf[ SENDMSG_BUFFER_SIZE ] = "";
	char *pBuf = szBuf;

	const size_t nCount = pPlayer->GetTitleConditionCount();
	if( SENDMSG_BUFFER_SIZE <= sizeof( TS_SC_TITLE_CONDITION_LIST ) + sizeof( TS_SC_TITLE_CONDITION_LIST::TS_TITLE_CONDITION_INFO ) * nCount )
		pBuf = new char[ sizeof( TS_SC_TITLE_CONDITION_LIST ) + sizeof( TS_SC_TITLE_CONDITION_LIST::TS_TITLE_CONDITION_INFO ) * nCount ];

	TS_SC_TITLE_CONDITION_LIST * pMsg = new (pBuf) TS_SC_TITLE_CONDITION_LIST;

	struct myTitleConditionFunctor : public StructTitleManager::TitleConditionFunctor
	{
		myTitleConditionFunctor( TS_SC_TITLE_CONDITION_LIST::TS_TITLE_CONDITION_INFO * pTitleConditionInfoBuffer ) : m_pTitleConditionInfoBuffer( pTitleConditionInfoBuffer ) {}

		virtual bool operator()( StructTitleCondition * pTitleCondition )
		{
			m_pTitleConditionInfoBuffer->type = pTitleCondition->GetType();
			m_pTitleConditionInfoBuffer->count = pTitleCondition->GetCount();

			++m_pTitleConditionInfoBuffer;

			return true;
		}

		TS_SC_TITLE_CONDITION_LIST::TS_TITLE_CONDITION_INFO * m_pTitleConditionInfoBuffer;
	} _atcfo( reinterpret_cast< TS_SC_TITLE_CONDITION_LIST::TS_TITLE_CONDITION_INFO * >( pMsg + 1 ) );

	pMsg->count = static_cast< unsigned short >( pPlayer->DoEachTitleCondition( _atcfo ) );
	pMsg->size += static_cast< unsigned int >( sizeof( TS_SC_TITLE_CONDITION_LIST::TS_TITLE_CONDITION_INFO ) * pMsg->count );

	// 메세지 전송
	PendStream( pPlayer, pMsg, pMsg->size );

	if( szBuf != pBuf )
		delete[] pBuf;
}

void SendRemainTitleTime( struct StructPlayer *pPlayer )
{
	AR_TIME t = GetArTime();

	TS_SC_REMAIN_TITLE_TIME msg;
	msg.remain_title_time = pPlayer->GetRemainTitleTime() > t ? pPlayer->GetRemainTitleTime() - t : 0;

	PendMessage( pPlayer, &msg );
}

void SendMainTitle( struct StructPlayer *pPlayer )
{
	TS_SC_SET_MAIN_TITLE msg;
	msg.handle = pPlayer->GetHandle();
	msg.code = pPlayer->GetMainTitle() ? pPlayer->GetMainTitle()->nID : 0;
	PendMessage( pPlayer, &msg );
}

void SendSubTitle( struct StructPlayer *pPlayer )
{
	for( int i = 0; i < GameRule::SUB_TITLE_COUNT; ++i )
	{
		TS_SC_SET_SUB_TITLE msg;
		msg.index = i;
		msg.code = pPlayer->GetSubTitle( i ) ? pPlayer->GetSubTitle( i )->nID : 0;
		PendMessage( pPlayer, &msg );
	}
}

void SendBoothClosedMessage( struct StructPlayer* pPlayer, AR_HANDLE target )
{
	TS_SC_BOOTH_CLOSED msg;
	msg.target = target;

	PendMessage( pPlayer, &msg );
}

void SendBoothInfo( struct StructPlayer* pPlayer, StructPlayer* pTarget )
{
	char szBuf[ sizeof( TS_SC_WATCH_BOOTH ) + ( sizeof( TS_SC_WATCH_BOOTH::TS_BOOTH_ITEM_INFO ) * GameRule::MAX_BOOTH_ITEM_COUNT ) ];
	TS_SC_WATCH_BOOTH * pMsg = new (szBuf) TS_SC_WATCH_BOOTH;

	pMsg->target = pTarget->GetHandle();
	pMsg->type = pTarget->GetBoothStatus();

	const std::vector< StructPlayer::BOOTH_ITEM_INFO > & rvItemList = pTarget->GetBoothItemList();
	
	pMsg->cnt = static_cast<unsigned short>( rvItemList.size() );
	pMsg->size += sizeof( TS_SC_WATCH_BOOTH::TS_BOOTH_ITEM_INFO ) * pMsg->cnt;

	TS_SC_WATCH_BOOTH::TS_BOOTH_ITEM_INFO * pBoothItemInfo = reinterpret_cast< TS_SC_WATCH_BOOTH::TS_BOOTH_ITEM_INFO * >( pMsg + 1 );

	for( std::vector< StructPlayer::BOOTH_ITEM_INFO >::const_iterator it = rvItemList.begin(); it != rvItemList.end(); ++it )
	{
		fillItemBaseInfo( pBoothItemInfo, (*it).pItem );
		pBoothItemInfo->count = (*it).cnt;
		pBoothItemInfo->gold = (*it).gold;

		++pBoothItemInfo;
	}

	PendStream( pPlayer, pMsg, pMsg->size );
}

void SendNPCStatusInVisibleRange( struct StructPlayer* pPlayer )
{
	struct _ArObjectFunctor : ArObjectFunctor
	{
		_ArObjectFunctor( StructPlayer *_pPlayer, ArcadiaIntf *_pIntf ) { pPlayer = _pPlayer; pIntf = _pIntf; }

		void operator()( ArObject *pObj ) const
		{
			GameObject::iterator it = GameObject::get( pObj->GetHandle() );

			if( !(*it) || !(*it)->IsNPC() )
			{
				return;
			}

			StructNPC * pNPC = static_cast< StructNPC * >( (*it) );
			
			TS_SC_STATUS_CHANGE msg;

			msg.handle = pNPC->GetHandle();
			msg.status = GetStatusCode( pNPC, pPlayer );

			pIntf->SendGameMessage( pPlayer, &msg );
		}

		ArcadiaIntf * pIntf;
		StructPlayer* pPlayer;

	} _sender( pPlayer, ArcadiaServer::GetIntf() );

	struct Sender : ArRegionFunctor
	{
		Sender( ArObjectFunctor *_pFo ) { pFo = _pFo; }

		void operator()( const struct ArRegion * pRegion )
		{
			pRegion->DoEachMovableObject( *pFo );
		}

		ArObjectFunctor * pFo;

	} _fo( &_sender );

	ArcadiaServer::Instance().DoEachVisibleRegion( pPlayer->GetRX(), pPlayer->GetRY(), pPlayer->GetLayer(), _fo );
}

void SendStatusMessage( struct StructPlayer * pClient, struct StructCreature* pCreature )
{
	TS_SC_STATUS_CHANGE msg;

	msg.handle = pCreature->GetHandle();
	msg.status = GetStatusCode( pCreature, pClient );

	PendMessage( pClient, &msg );
}

void BroadcastStatusMessage( struct StructCreature* pCreature )
{
	struct _ArObjectFunctor : ArObjectFunctor
	{
		_ArObjectFunctor( StructCreature *_pCreature, ArcadiaIntf *_pIntf ) { pCreature = _pCreature; pIntf = _pIntf; }

		void operator()( ArObject *pObj ) const
		{
			TS_SC_STATUS_CHANGE msg;

			StructPlayer * pClient = static_cast< StructPlayer* >( pObj );

			msg.handle = pCreature->GetHandle();
			msg.status = GetStatusCode( pCreature, pClient );

			pIntf->SendGameMessage( pObj, &msg );
		}

		ArcadiaIntf * pIntf;
		StructCreature* pCreature;

	} _sender( pCreature, ArcadiaServer::GetIntf() );

	struct Sender : ArRegionFunctor
	{
		Sender( ArObjectFunctor *_pFo ) { pFo = _pFo; }

		void operator()( const struct ArRegion * pRegion )
		{
			pRegion->DoEachClient( *pFo );
		}

		ArObjectFunctor * pFo;

	} _fo( &_sender );

	ArcadiaServer::Instance().DoEachVisibleRegion( pCreature->GetRX(), pCreature->GetRY(), pCreature->GetLayer(), _fo );
}

void BroadcastHairInfo( struct StructPlayer * pPlayer )
{
	TS_SC_HAIR_INFO msg;

	msg.hPlayer = pPlayer->GetHandle();
	msg.nHairID = pPlayer->GetBaseModelId( 0 );
	msg.nHairColorIndex = pPlayer->GetHairColorIndex();
	msg.nHairColorRGB = pPlayer->GetHairColorRGB();

	ArcadiaServer::Instance().Broadcast( pPlayer->GetRX(), pPlayer->GetRY(), pPlayer->GetLayer(), &msg );
}

void BroadcastSkinInfo( struct StructPlayer * pPlayer )
{
	TS_SC_SKIN_INFO msg;

	msg.hPlayer = pPlayer->GetHandle();
	msg.nSkinColor = pPlayer->GetSkinColor();

	ArcadiaServer::Instance().Broadcast( pPlayer->GetRX(), pPlayer->GetRY(), pPlayer->GetLayer(), &msg );
}

// 이하 인증 관련 메시지
void SendGameServerLoginToAuth( const char * szServerName, const char * szServerScreenshotUrl, unsigned short nServerIdx, const char * szServerIP, int nServerPort )
{
	TS_GA_LOGIN msg;
	s_strcpy( msg.server_name, _countof( msg.server_name ), szServerName );
	s_strcpy( msg.server_screenshot_url, _countof( msg.server_screenshot_url ), szServerScreenshotUrl );
	s_strcpy( msg.server_ip, _countof( msg.server_ip ), szServerIP );

	msg.server_port = nServerPort;
	msg.is_adult_server = GameRule::bIsAdultServer;

	msg.server_idx = nServerIdx;

	if( g_pAuthConnection && g_pAuthConnection->IsConnected() )
	{
		PendMessage( g_pAuthConnection, &msg );
	}
}

// 이하 업로드 관련 메시지
void SendGameServerLoginToUpload( const char *szServerName )
{
	TS_SU_LOGIN msg;
	s_strcpy( msg.server_name, _countof( msg.server_name ), szServerName );

	if( g_pUploadConnection && g_pUploadConnection->IsConnected() )
	{
		PendMessage( g_pUploadConnection, &msg );
	}
}

void fillAuctionMessageBuffer( struct TS_AUCTION_INFO *pBuffer, const class AuctionInfo *pAuctionInfo )
{
	pBuffer->auction_uid = pAuctionInfo->nAuctionID;
	pBuffer->bidded_price = pAuctionInfo->nHighestBiddingPrice.GetRawData();

	time_t tLeftDuration = pAuctionInfo->tAuctionEnd - time( NULL );
	if( tLeftDuration > GameRule::AUCTION_DURATION_MIDTERM )
		pBuffer->duration_type = TS_AUCTION_INFO::DURATION_LONGTERM;
	else if( tLeftDuration > GameRule::AUCTION_DURATION_SHORTTERM )
		pBuffer->duration_type = TS_AUCTION_INFO::DURATION_MIDTERM;
	else
		pBuffer->duration_type = TS_AUCTION_INFO::DURATION_SHORTTERM;

	pBuffer->instant_purchase_price = pAuctionInfo->nInstantPurchasePrice.GetRawData();
	fillItemBaseInfo( &pBuffer->item_info, pAuctionInfo->pItem );
}

bool SendLoginToAuth( const char * szAccount, __int64 one_time_key )
{
	if( GameRule::bUseLoginLogoutDebug )
	{
		_ctprint( "SendLoginToAuth (%s)\n", szAccount );
		FileLogHandler::GetFileLogHandler()->LogStringEx( NULL, "LoginLogout_Debug", "SendLoginToAuth (%s)", szAccount );
	}

	TS_GA_CLIENT_LOGIN msg;

	s_strcpy( msg.account, _countof( msg.account ), szAccount );
	msg.one_time_key = one_time_key;

	if( g_pAuthConnection && g_pAuthConnection->IsConnected() )
	{
		PendMessage( g_pAuthConnection, &msg );
		return true;
	}

	_cprint( "authserver is not connected: %s\n", szAccount );
	FILELOG( "authserver is not connected: %s", szAccount );
	return false;
}

void SendLogoutToAuth( const char * szAccount, AR_TIME nContinuousPlayTime, const int nCallerIdx )
{
	if( GameRule::bUseLoginLogoutDebug )
	{
		_ctprint( "SendLogoutToAuth (%s, %d)\n", szAccount, nCallerIdx );
		FileLogHandler::GetFileLogHandler()->LogStringEx( NULL, "LoginLogout_Debug", "SendLogoutToAuth (%s, %d)", szAccount, nCallerIdx );
	}

	TS_GA_CLIENT_LOGOUT msg;

	s_strcpy( msg.account, _countof( msg.account ), szAccount );
	msg.nContinuousPlayTime = nContinuousPlayTime;

	if( g_pAuthConnection && g_pAuthConnection->IsConnected() )
	{
		PendMessage( g_pAuthConnection, &msg );
	}
}

void SendGameTime( const ArObject * pClient )
{
	TS_SC_GAME_TIME msg;

	msg.t = GetArTime();
	msg.game_time = time( NULL );

	PendMessage( pClient, &msg );
}

void SendHPMPMsg( const struct StructPlayer * pPlayer, const struct StructCreature * pCreature, int add_hp, short add_mp, bool need_to_display )
{
	TS_SC_HPMP msg;

	msg.handle = pCreature->GetHandle();

	msg.add_hp = add_hp;
	msg.add_mp = add_mp;
	msg.hp = pCreature->GetHP();
	msg.mp = pCreature->GetMP();
	msg.max_hp = pCreature->GetMaxHP();
	msg.max_mp = pCreature->GetMaxMP();
	msg.need_to_display = need_to_display;

	PendMessage( pPlayer, &msg );
}

void BroadcastHPMPMsg( const struct StructCreature *pCreature, int add_hp, short add_mp, bool need_to_display )
{
	TS_SC_HPMP msg;

	msg.handle = pCreature->GetHandle();

	msg.add_hp = add_hp;
	msg.add_mp = add_mp;
	msg.hp = pCreature->GetHP();
	msg.mp = pCreature->GetMP();
	msg.max_hp = pCreature->GetMaxHP();
	msg.max_mp = pCreature->GetMaxMP();
	msg.need_to_display = need_to_display;

	ArcadiaServer::Instance().Broadcast( pCreature->GetRX(), pCreature->GetRY(), pCreature->GetLayer(), &msg );
}

void SendOpenUrl( struct StructPlayer * pPlayer, const char *szUrl, const bool bWaitForEventScene, const int width, const int height )
{
	if (!szUrl || szUrl[0] == 0)
		return;

	TS_SC_OPEN_URL msg;

	msg.wait_for_event_scene = bWaitForEventScene;
	msg.width = width;
	msg.height = height;
	msg.url_len = static_cast<unsigned short>( strlen( szUrl ) );
	msg.size += msg.url_len;

	if( msg.size > 1024 )
		return;

	char szTmp[ 1024 ];
	char *p = szTmp + sizeof(msg);
	int temp_size = sizeof(szTmp) - sizeof(msg);
	
	s_memcpy( p, temp_size, szUrl, msg.url_len );
	p += msg.url_len;
	temp_size -= msg.url_len;

	s_memcpy( szTmp, sizeof( szTmp ), &msg, sizeof(msg) );

	PendStream( pPlayer, szTmp, msg.size );
}

void SendUrlList( struct StructPlayer * pPlayer )
{
	TS_SC_URL_LIST msg;

	if( !ENV().IsExist( "game.url_list" ) )
		return;

	std::string szUrlList = ENV().GetString( "game.url_list" ) + "|guild_icon_upload.url|" + ENV().GetString( "game.guild_icon_base_url" ) + "/iconupload.aspx";

	msg.url_list_len = static_cast<unsigned short>( szUrlList.length() );
	msg.size += msg.url_list_len;

	if( msg.size > 1024 )
		return;

	char szTmp[ 1024 ];
	char *p = szTmp;
	int temp_size = sizeof( szTmp );
	
	s_memcpy( p, temp_size, &msg, sizeof(msg) );					p += sizeof(msg); temp_size -= sizeof(msg);
	s_memcpy( p, temp_size, szUrlList.c_str(), msg.url_list_len );	p += msg.url_list_len; temp_size -= msg.url_list_len;

	PendStream( pPlayer, szTmp, msg.size );
}

void SendShowCreatureNameChangeBox( struct StructPlayer *pPlayer, AR_HANDLE hSummon )
{
	if( !pPlayer )
		return;

	ARCADIA_LOCK( ArcadiaServer::Instance().LockObjectWithVisibleRange( pPlayer ) );

	pPlayer->SetLastContact( "creature_name_change", 1 );

	TS_SC_SHOW_SUMMON_NAME_CHANGE msg;

	msg.handle = hSummon;

	PendMessage( pPlayer, &msg );
}

void SendHuntaholicInstanceInfo( const struct StructPlayer * pPlayer, const TS_HUNTAHOLIC_INSTANCE_INFO & info )
{
	if( !pPlayer )
		return;

	TS_SC_HUNTAHOLIC_INSTANCE_INFO msg;

	s_memcpy( &msg.info, sizeof( msg.info ), &info, sizeof( msg.info ) );

	PendMessage( pPlayer, &msg );
}

void SendHuntaholicHuntingScore( const struct StructPlayer * pPlayer, const int nHuntaholicID, const int nPersonalKillCount, const int nPersonalScore, const int nKillCount, const int nScore, const double & fPointAdvantage, const int nGainPoint, const char nResultType )
{
	if( !pPlayer )
		return;

	TS_SC_HUNTAHOLIC_HUNTING_SCORE msg;

	msg.huntaholic_id = nHuntaholicID;
	msg.personal_kill_count = nPersonalKillCount;
	msg.personal_score = nPersonalScore;
	msg.kill_count = nKillCount;
	msg.score = nScore;
	msg.point_advantage = fPointAdvantage;
	msg.point_rate = 1.0;
	msg.gain_point = nGainPoint;
	msg.result_type = nResultType;

	PendMessage( pPlayer, &msg );
}

void SendCompeteRequest( const struct StructPlayer * pPlayer, const unsigned char nCompeteType, const char * szRequester )
{
	if( !pPlayer )
		return;

	TS_SC_COMPETE_REQUEST msg;
	msg.compete_type = nCompeteType;
	s_strcpy( msg.requester, _countof( msg.requester ), szRequester );

	PendMessage( pPlayer, &msg );
}

void SendCompeteAnswer( const struct StructPlayer * pPlayer, const unsigned char nCompeteType, const unsigned char nAnswerType, const char * szRequestee )
{
	if( !pPlayer )
		return;

	TS_SC_COMPETE_ANSWER msg;
	msg.compete_type = nCompeteType;
	msg.answer_type = nAnswerType;
	s_strcpy( msg.requestee, _countof( msg.requestee ), szRequestee );

	PendMessage( pPlayer, &msg );
}

void SendCompeteCountdown( const struct StructPlayer * pPlayer, const unsigned char nCompeteType, const char * szCompetitor, const AR_HANDLE hCompetitor )
{
	if( !pPlayer )
		return;

	TS_SC_COMPETE_COUNTDOWN msg;
	msg.compete_type = nCompeteType;
	s_strcpy( msg.competitor, _countof( msg.competitor ), szCompetitor );
	msg.handle_competitor = hCompetitor;

	PendMessage( pPlayer, &msg );
}

void SendCompeteStart( const struct StructPlayer * pPlayer, const unsigned char nCompeteType, const char * szCompetitor )
{
	if( !pPlayer )
		return;

	TS_SC_COMPETE_START msg;
	msg.compete_type = nCompeteType;
	s_strcpy( msg.competitor, _countof( msg.competitor ), szCompetitor );

	PendMessage( pPlayer, &msg );
}

void SendCompeteEnd( const struct StructPlayer * pPlayer, const unsigned char nCompeteType, const unsigned char nEndType, const char * szWinner, const char * szLoser )
{
	if( !pPlayer )
		return;

	TS_SC_COMPETE_END msg;
	msg.compete_type = nCompeteType;
	msg.end_type = nEndType;
	s_strcpy( msg.winner, _countof( msg.winner ), szWinner );
	s_strcpy( msg.loser, _countof( msg.loser ), szLoser );

	PendMessage( pPlayer, &msg );
}
