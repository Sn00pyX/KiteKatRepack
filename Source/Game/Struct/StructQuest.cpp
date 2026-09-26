
#include <mmo/ArcadiaServer.h>
#include <toolkit/khash.h>
#include <toolkit/XConsole.h>
#include <logging/FileLog.h>

#ifdef _MEM_USAGE_DEBUG
#include <dump/XExceptionHandler.h>
#endif

#include "StructQuest.h"
#include "GameAllocator.h"


static std::vector< QuestBaseServer >							s_vQuestBase;
static KHash< size_t, hashPr_mod_basic<QuestBase::QuestCode>  >	s_hsQuestCode;


StructQuest * StructQuest::AllocQuest( struct QuestEventHandler *pHandler, int nId, QuestBase::QuestCode code, int status[], const AR_TIME nTimeLimit, QuestInstance::QUEST_PROGRESS progress, int nStartID )
{
	StructQuest *pObj = allocQuestStruct();
	if( !pObj )
		return NULL;

	new (pObj) StructQuest( pHandler, nId, code, status, nTimeLimit, progress, nStartID );

	return pObj;
}

void StructQuest::FreeQuest()
{
	prepareFreeQuestStruct( this );
	StructQuest::~StructQuest();
	freeQuestStruct( this );
}

bool StructQuest::RegisterQuestBase( const QuestBaseServer & base )
{
	// 이미 존재하면 수정
	if( s_hsQuestCode.has( base.nCode ) )
	{
		size_t idx;
		s_hsQuestCode.lookup( base.nCode, idx );
		s_vQuestBase[idx] = base;
		
		return true;
	}

	s_vQuestBase.push_back( base );

	s_hsQuestCode.add( base.nCode, s_vQuestBase.size()-1 );

	return true;
}

StructQuest::StructQuest(	struct QuestEventHandler *pHandler,
							int nId,
							QuestBase::QuestCode code,
							int status[],
							const AR_TIME nTimeLimit,
							QuestInstance::QUEST_PROGRESS progress,
							int nStartID )
{
	m_pHandler = pHandler;
	m_bIsNeedUpdateToDB = false;
	m_pQuestBase = &GetQuestBase( code );
	m_Instance.nId = nId;
	m_Instance.Code = code;
	m_Instance.nStartID = nStartID;

	for( int i = 0; i < QuestInstance::MAX_STATUS; ++i )
	{
		m_Instance.nStatus[ i ] = status[ i ];
	}

	m_Instance.nTimeLimit = nTimeLimit;
	m_Instance.nProgress = progress;

#ifdef _MEM_USAGE_DEBUG
	XSEH::IncreaseAllocCount( "StructQuest" );
#endif
}

StructQuest::~StructQuest()
{
#ifdef _MEM_USAGE_DEBUG
	XSEH::DecreaseAllocCount( "StructQuest" );
#endif
}

const QuestBaseServer & StructQuest::GetQuestBase( QuestBase::QuestCode c )
{
	static QuestBaseServer temp;

	size_t idx;
	if( !s_hsQuestCode.lookup( c, idx ) )
	{
		FILELOG( "invalid quest code! code[%d]", c );
		_cprint( "invalid quest code! code[%d]\n", c );

		return temp;
	}

	return s_vQuestBase[ idx ];
}

bool StructQuest::IsFinishable() const
{
	if( m_Instance.nProgress != QuestInstance::IN_PROGRESS && m_Instance.nProgress != QuestInstance::FINISHABLE )
		return false;

	bool bRet = false;

	switch( GetQuestType() )
	{
	case QuestBase::QUEST_KILL_TOTAL:
		if( GetStatus( 0 ) >= GetValue( 1 ) )
		{
			bRet = true;
		}
		break;

	case QuestBase::QUEST_KILL_INDIVIDUAL:
		if( GetStatus( 0 ) >= GetValue( 1 ) && GetStatus( 1 ) >= GetValue( 3 ) && GetStatus( 2 ) >= GetValue( 5 ) )
		{
			bRet = true;
		}
		break;
	case QuestBase::QUEST_RANDOM_KILL_INDIVIDUAL:
		if( GetStatus( 0 ) >= GetRandomValue( 0 ) && GetStatus( 1 ) >= GetRandomValue( 1 ) && GetStatus( 2 ) >= GetRandomValue( 2 ) )
		{
			bRet = true;
		}
		break;
	case QuestBase::QUEST_LEARN_SKILL:
		if( ( !GetValue( 0 ) || GetStatus( 0 ) >= GetValue( 1 ) ) && 
			( !GetValue( 2 ) || GetStatus( 1 ) >= GetValue( 3 ) ) && 
			( !GetValue( 4 ) || GetStatus( 2 ) >= GetValue( 5 ) ) )
		{
			bRet = true;
		}
		break;
	case QuestBase::QUEST_JOB_LEVEL:
		if( GetStatus( 0 ) >= GetValue( 0 ) && GetStatus( 1 ) >= GetValue( 1 ) )
		{
			bRet = true;
		}
		break;
	case QuestBase::QUEST_PARAMETER:	// 참 공식 난해하다.-_-
		if( ( !GetValue( 0 )	|| ( GetValue( 1 ) == 2 && GetStatus( 0 ) > GetValue( 2 ) )
								|| ( GetValue( 1 ) == 1 && GetStatus( 0 ) >= GetValue( 2 ) )
								|| ( GetValue( 1 ) == 0 && GetStatus( 0 ) == GetValue( 2 ) )
								|| ( GetValue( 1 ) == -1 && GetStatus( 0 ) <= GetValue( 2 ) )
								|| ( GetValue( 1 ) == -2 && GetStatus( 0 ) < GetValue( 2 ) )
								) &&
			( !GetValue( 3 )	|| ( GetValue( 4 ) == 2 && GetStatus( 1 ) > GetValue( 5 ) )
								|| ( GetValue( 4 ) == 1 && GetStatus( 1 ) >= GetValue( 5 ) )
								|| ( GetValue( 4 ) == 0 && GetStatus( 1 ) == GetValue( 5 ) )
								|| ( GetValue( 4 ) == -1 && GetStatus( 1 ) <= GetValue( 5 ) )
								|| ( GetValue( 4 ) == -2 && GetStatus( 1 ) < GetValue( 5 ) )
								) &&
			( !GetValue( 6 )	|| ( GetValue( 7 ) == 2 && GetStatus( 2 ) > GetValue( 8 ) )
								|| ( GetValue( 7 ) == 1 && GetStatus( 2 ) >= GetValue( 8 ) )
								|| ( GetValue( 7 ) == 0 && GetStatus( 2 ) == GetValue( 8 ) )
								|| ( GetValue( 7 ) == -1 && GetStatus( 2 ) <= GetValue( 8 ) )
								|| ( GetValue( 7 ) == -2 && GetStatus( 2 ) < GetValue( 8 ) )
								) )
		{
			bRet = true;
		}
		break;
	case QuestBase::QUEST_HUNT_ITEM:
	case QuestBase::QUEST_HUNT_ITEM_FROM_ANY_MONSTERS:
	case QuestBase::QUEST_HUNT_ITEM_DROP_PENALTY:
		if( GetStatus( 0 ) >= GetValue( 1 ) && GetStatus( 1 ) >= GetValue( 3 ) && GetStatus( 2 ) >= GetValue( 5 ) )
		{
			bRet = true;
		}
		break;
	case QuestBase::QUEST_KILL_PLAYER:
		if( GetStatus( 0 ) >= GetValue( 1 ) && GetStatus( 1 ) >= GetValue( 3 ) )
		{
			bRet = true;
		}
		break;
	case QuestBase::QUEST_UPGRADE_ITEM:
		if( GetStatus( 0 ) >= GetValue( 1 ) && GetStatus( 1 ) >= GetValue( 3 ) && GetStatus( 2 ) >= GetValue( 5 ) )
		{
			bRet = true;
		}
		break;
	case QuestBase::QUEST_ENHANCE_ITEM:
		if( GetStatus( 0 ) >= GetValue( 1 ) && GetStatus( 1 ) >= GetValue( 3 ) && GetStatus( 2 ) >= GetValue( 5 ) )
		{
			bRet = true;
		}
		break;
	case QuestBase::QUEST_COLLECT:
	case QuestBase::QUEST_EXTERNAL_CONTROL:
		if( GetStatus( 0 ) >= GetValue( 1 ) && GetStatus( 1 ) >= GetValue( 3 ) && GetStatus( 2 ) >= GetValue( 5 ) &&
			GetStatus( 3 ) >= GetValue( 7 ) && GetStatus( 4 ) >= GetValue( 9 ) && GetStatus( 5 ) >= GetValue( 11 ) )
		{
			bRet = true;
		}
		break;
	case QuestBase::QUEST_RANDOM_COLLECT:
		if( GetStatus( 0 ) >= GetRandomValue( 0 ) && GetStatus( 1 ) >= GetRandomValue( 1 ) && GetStatus( 2 ) >= GetRandomValue( 2 ) )
		{
			bRet = true;
		}
		break;
	case QuestBase::QUEST_CONTACT:
		bRet = true;
		break;

	default:
		break;
	}

	return bRet;
}

const int StructQuest::GetRemainTime() const
{
	return ( GetTimeLimitType() != QuestBase::TIME_LIMIT_TYPE_PERMANENT ) ? std::max( ( GetTimeLimit() - GetArTime() ) / 100, static_cast< AR_TIME >( 0 ) ) : 0;
}

void StructQuest::SetTimeLimit( const AR_TIME & nTimeLimit )
{
	assert( GetTimeLimitType() != QuestBase::TIME_LIMIT_TYPE_PERMANENT || !nTimeLimit );
	m_Instance.nTimeLimit = nTimeLimit;
}

int StructQuest::GetValue( int idx ) const
{
	if( idx < 0 || idx > QuestBase::MAX_VALUE_NUMBER - 1 )
	{
		FILELOG( "StructQuest::GetValue invalid index[%d]!", idx );
		_cprint( "StructQuest::GetValue invalid index[%d]!\n", idx );

		return 0;
	}

	return GetQuestBase().nValue[ idx ];
}

int StructQuest::GetStatus( int idx ) const
{
	if( idx < 0 || idx > QuestInstance::MAX_STATUS - 1 )
	{
		FILELOG( "StructQuest::GetStatus invalid index[%d]!", idx );
		_cprint( "StructQuest::GetStatus invalid index[%d]!\n", idx );

		return 0;
	}

	return m_Instance.nStatus[ idx ];
}

int StructQuest::GetRandomKey( int idx ) const
{
	return m_Instance.nRandomKey[idx];
}

int StructQuest::GetRandomValue( int idx ) const
{
	return m_Instance.nRandomValue[idx];
}

void StructQuest::SetRandomKey( int idx, int value )
{
	m_Instance.nRandomKey[idx] = value;
}

void StructQuest::SetRandomValue( int idx, int value )
{
	m_Instance.nRandomValue[idx] = value;
}

void StructQuest::IncStatus( int idx, int value )
{
	if( idx < 0 || idx > QuestInstance::MAX_STATUS - 1 )
	{
		FILELOG( "StructQuest::IncStatus invalid index[%d]!", idx );
		_cprint( "StructQuest::IncStatus invalid index[%d]!\n", idx );

		return;
	}

	m_Instance.nStatus[ idx ] = m_Instance.nStatus[ idx ] + value;

	if( m_pHandler ) m_pHandler->onStatusChanged( this );

	TurnOnUpdateFlag();
}

void StructQuest::UpdateStatus( int idx, int value )
{
	if( idx < 0 || idx > QuestInstance::MAX_STATUS - 1 )
	{
		FILELOG( "StructQuest::UpdateStatus invalid index[%d]!", idx );
		_cprint( "StructQuest::UpdateStatus invalid index[%d]!\n", idx );

		return;
	}

	m_Instance.nStatus[ idx ] = value;

	if( m_pHandler ) m_pHandler->onStatusChanged( this );

	TurnOnUpdateFlag();
}

void StructQuest::ResetStatus()
{
	memset( m_Instance.nStatus, 0, sizeof( m_Instance.nStatus ) );

	if( m_pHandler ) m_pHandler->onStatusChanged( this );

	TurnOnUpdateFlag();
}

const QuestBase::Reward & StructQuest::GetOptionalReward( int idx ) const
{
	static QuestBase::Reward tmp;
	tmp.nItemCode = 0;
	tmp.nQuantity = 0;

	if( idx < 0 || idx > QuestBase::MAX_OPTIONAL_REWARD - 1 )
	{
		FILELOG( "StructQuest::GetOptionalReward invalid index[%d]!", idx );
		_cprint( "StructQuest::GetOptionalReward invalid index[%d]!\n", idx );

		return tmp;
	}
	return GetQuestBase().OptionalReward[ idx ];
}

bool StructQuest::IsRandomQuest( int code )
{
	int quest_type = GetQuestBase( code ).nType;

	if( quest_type == QuestBase::QUEST_RANDOM_COLLECT || quest_type == QuestBase::QUEST_RANDOM_KILL_INDIVIDUAL )
		return true;

	return false;
}