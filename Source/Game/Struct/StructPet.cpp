
#include <toolkit/XEnv.h>
#include <mmo/ArcadiaServer.h>
#include <toolkit/XRandom.h>
#include <toolkit/XSTLUtil.h>

#include "LogClient/LogClient.h"
#include "ErrorCode/ErrorCode.h"

#include "StructPet.h"
#include "SendMessage.h"
#include "StructItem.h"
#include "StructPlayer.h"
#include "StructMonster.h"
#include "DB_Commands.h"
#include "StructSkill.h"
#include "GameProc.h"
#include "GameMessage.h"
#include "GameAllocator.h"


void StructPet::BindProperty()
{
	StructPet temp( "for bind property" );
	temp.BindCString( "name", temp.m_szName, _countof( temp.m_szName ) );
	temp.Bind( "x", &temp.mv.x );
	temp.Bind( "y", &temp.mv.y );
	temp.Bind( "summon_state", &temp.m_bIsSummoned );
}

StructPet* StructPet::AllocPet( struct StructPlayer* pMaster, unsigned int code )
{
	StructPet* p;

	struct _myIntializer : GameAllocateFunctor
	{
		_myIntializer( int code ) : m_nCode( code ) {}
		
		virtual void operator()( void * pObj, AR_HANDLE handle )
		{
			new (pObj) StructPet( handle, m_nCode );
		}

		unsigned m_nCode;
	};

	AR_HANDLE handle = allocPetStruct( &p, _myIntializer( code ) );
	p->m_pMaster = pMaster;

	// 삽질 스킬 넣어줘야 함
	p->SetSkill( StructSkill::SKILL_UID_PET_SKILL, StructSkill::SKILL_SHOVELING, 1, 0 );

	return p;
}

void StructPet::FreePet( StructPet* p )
{
	// _oprint( "DEL Pet : %08X\n", p );

	prepareFreePetStruct( p );

	p->StructPet::~StructPet();

	freePetStruct( p );
}

bool StructPet::SetPetInfo( unsigned int code )
{
	m_pContentInfo = GameContent::GetPetInfo( code );

	return (m_pContentInfo != NULL);
}

void StructPet::SetName( const char *szName )
{
	if ( strnlen( szName, _countof(m_szName) ) < _countof(m_szName) )
	{
		s_strcpy( m_szName, _countof(m_szName), szName );
	}
	else
	{
		std::string codePage("."+ENV().GetString( "CodePage", "0" )); // CP_ACP = 0

		_mbsnbcpy_s_l((unsigned char*)m_szName, sizeof( m_szName ), (unsigned char*)szName, sizeof(m_szName)-1, _create_locale(LC_ALL, codePage.c_str()));
	}	
}

void StructPet::SetName( const int nNameId )
{
	SetName( GameContent::GetString( nNameId ) );
}

const unsigned short StructPet::ChangeName( const char *szName, const bool bForce )
{
	if( !bForce && m_bNameChanged )
		return RESULT_NOT_ACTABLE;

	if( strlen( szName ) > 18 )
		return RESULT_LIMIT_MAX;

	if( strlen( szName ) < 4 )
		return RESULT_LIMIT_MIN;

	int code_page = ENV().GetInt( "CodePage", CP_ACP );
	if( !GameRule::IsValidName( code_page, szName, static_cast<int>( strlen( szName ) ) + 1, 4, 18 ) ||
		GameContent::IsBannedWord( code_page, szName ) )
		return RESULT_ACCESS_DENIED;

	if( !strcmp( m_szName, szName ) )
		return RESULT_ALREADY_EXIST;

	m_bNameChanged = true;

	s_strcpy( m_szName, _countof( m_szName ), szName );

	TS_SC_CHANGE_NAME msg;

	msg.handle = GetHandle();
	s_strcpy( msg.name, _countof( msg.name ), m_szName );

	if( m_pMaster )
	{
		PendMessage( m_pMaster, &msg );
	}

	if( IsInWorld() )
	{
		ArcadiaServer::Instance().Broadcast( GetRX(), GetRY(), GetLayer(), &msg );	
	}

	DBQuery( new DB_ChangePetName( this ) );

	return RESULT_SUCCESS;
}

StructPet::StructPet( AR_HANDLE handle, unsigned idx )
	: m_bQueryLock( "StructPet::m_bQueryLock" )
{
	m_hHandle = handle;
	m_nSID = 0;
	m_bNameChanged = true;

	m_nArObjectType = ArObject::MOVABLE_OBJECT;

	SetPetInfo( idx );

	m_bIsSummoned = false;

	m_pMaster = NULL;
	m_pItem = NULL;

	m_nHP = 100;
	m_nMaxHP = 100;
	m_nMP = 100;
	m_nMaxMP = 100;

	memset( m_szName, 0, sizeof(m_szName) );

	m_nLastProcessTime = 0;

	m_nShovelingStatus = SHOVELING_STATUS_IDLE;

	// 임시 : 랜덤하게 다른방향 보게 하자.. -_-;
	mv.face = (float)XRandom() / 100;

#ifdef _MEM_USAGE_DEBUG
	XSEH::IncreaseAllocCount( "StructPet" );
#endif
}

StructPet::~StructPet()
{
	if( IsInWorld() ) 
	{
		ARCADIA_LOCK( ArcadiaServer::Instance().LockObjectWithVisibleRange( this ) );
		RemovePetFromWorld( this );
	}

#ifdef _MEM_USAGE_DEBUG
	XSEH::DecreaseAllocCount( "StructPet" );
#endif
}

bool StructPet::ProcDelete()
{
	FreePet( this );

	return true;
}

void StructPet::onChangeProperty( const std::string & strKey, const char *data )
{
	const char *szKey = strKey.c_str();

	SendPropertyMessage( GetMaster(), GetHandle(), szKey, GetAsString( szKey ).c_str() );
}

void StructPet::DBQuery( GameDBManager::DBProc *pWork )
{
	THREAD_SYNCRONIZE( m_bQueryLock );

	if( m_lQueryList.empty() )
	{
		DB().Push( pWork );
	}

	m_lQueryList.push_back( pWork );
}

void StructPet::onEndQuery()
{
	THREAD_SYNCRONIZE( m_bQueryLock );

	m_lQueryList.pop_front();

	if( !m_lQueryList.empty() )
	{
		DB().Push( m_lQueryList.front() );
	}
}

bool StructPet::IsDeleteable()
{
	THREAD_SYNCRONIZE( m_bQueryLock );

	// 스케쥴러에 등록되어 있으면 삭제 불가 && 다른 쓰레드에서 참조중이면 삭제 불가
	if( !GameObject::IsDeleteable() ) return false;

	// DB 쿼리 미결된게 있다면 삭제 불가
	if( !m_lQueryList.empty() ) return false;

	return true;
}

const int StructPet::GetAddedWeight() const 
{ 
	if (!m_pItem || !m_pItem->GetInstanceFlag().IsOn(ItemInstance::ITEM_FLAG_ENHANCE))
	{
		return ((m_pContentInfo->attribute_flag & PetBase::RATE_WEIGHT_MAX) >> PetBase::RATE_WEIGHT_INDEX) * 1000;
	}
	return 3000;
}