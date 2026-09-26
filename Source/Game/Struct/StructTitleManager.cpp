
#include <toolkit/khash.h>
#include <dump/XException.h>

#include "StructTitleManager.h"
#include "TitleBase.h"
#include "StructTitleCondition.h"
#include "GameRule.h"
#include "StructItem.h"


static std::vector< TitleBaseServer * >							s_vTitleBase;
static KHash< size_t, hashPr_mod_int >							s_hsTitleID;
static std::vector< TitleCondition * >							s_vTitleCondition;
static KHash< size_t, hashPr_mod_int >							s_hsTitleCondition;
static std::vector< TitleConditionType * >						s_vTitleConditionType;
static KHash< size_t, hashPr_mod_int >							s_hsTitleConditionTypeByID;
static KHash< std::pair< size_t, size_t > *, hashPr_mod_int >	s_hsTitleConditionTypeByType;

static volatile LONG			s_nTitleMaxID;
static volatile LONG			s_nTitleConditionMaxID;

StructTitleManager::StructTitleManager()
{
	m_pHandler = NULL;
}

StructTitleManager::~StructTitleManager()
{
	m_hsTitle.clear();
	m_hsTitleCondition.clear();

	for( std::vector< StructTitle * >::iterator it = m_vTitle.begin(); it != m_vTitle.end(); ++it )
	{
		(*it)->FreeTitle();
	}
	m_vTitle.clear();

	for( std::vector< StructTitleCondition * >::iterator it = m_vTitleCondition.begin(); it != m_vTitleCondition.end(); ++it )
	{
		(*it)->FreeTitleCondition();
	}
	m_vTitleCondition.clear();
}

void StructTitleManager::AddTitle( StructTitle * pTitle )
{
	m_vTitle.push_back( pTitle );
	m_hsTitle.add( pTitle->GetCode(), m_vTitle.size() - 1 );

	if( pTitle->GetStatus() & StructTitle::TITLE_STATUS_ACHIEVE )
		UpdateTitleConditionByTitleAchieve( pTitle->GetCode() );
}

bool StructTitleManager::AddTitleCondition( struct StructTitleCondition * pTitleCondition )
{
	m_vTitleCondition.push_back( pTitleCondition );
	m_hsTitleCondition.add( pTitleCondition->GetType(), m_vTitleCondition.size() - 1 );
	return true;
}

void StructTitleManager::SetTitleMaxID( int nID )
{
	if( s_nTitleMaxID )
	{
		throw XException( "Call SetTitleMaxID twice by a manager." );
	}

	s_nTitleMaxID = nID;
}

void StructTitleManager::SetTitleConditionMaxID( int nID )
{
	if( s_nTitleConditionMaxID )
	{
		throw XException( "Call SetTitleConditionMaxID twice by a manager." );
	}

	s_nTitleConditionMaxID = nID;
}

int	StructTitleManager::AllocTitleID()
{
	return InterlockedIncrement( &s_nTitleMaxID );
}

int	StructTitleManager::AllocTitleConditionID()
{
	return InterlockedIncrement( &s_nTitleConditionMaxID );
}

TitleBaseServer * StructTitleManager::RegisterTitleBase( TitleBaseServer * base )
{
	if( s_hsTitleID.has( base->nID ) )
	{
		size_t idx;
		s_hsTitleID.lookup( base->nID, idx );
		s_vTitleBase[idx] = base;

		return s_vTitleBase[idx];
	}

	s_vTitleBase.push_back( base );
	s_hsTitleID.add( base->nID, s_vTitleBase.size() - 1 );

	return s_vTitleBase.back();
}

TitleCondition * StructTitleManager::RegisterTitleCondition( TitleCondition * base )
{
	if( s_hsTitleCondition.has( base->nTitleID ) )
	{
		size_t idx;
		s_hsTitleCondition.lookup( base->nTitleID, idx );
		s_vTitleCondition[idx] = base;

		return s_vTitleCondition[idx];
	}

	s_vTitleCondition.push_back( base );
	s_hsTitleCondition.add( base->nTitleID, s_vTitleCondition.size() - 1 );

	return s_vTitleCondition.back();
}

void StructTitleManager::RegisterTitleConditionType( TitleConditionType * base )
{
	static int nLastCategory = 0;
	static int nLastCount = 0;

	if( nLastCategory != base->nCategory )
	{
		if( nLastCategory )
		{
			std::pair< size_t, size_t > * pRange = new std::pair< size_t, size_t >( s_vTitleConditionType.size() - nLastCount, nLastCount );
			s_hsTitleConditionTypeByType.add( nLastCategory, pRange );
		}

		nLastCategory = base->nCategory;
		nLastCount = 0;
	}

	if( !base->nID )
		return;

	s_vTitleConditionType.push_back( base );
	s_hsTitleConditionTypeByID.add( base->nID, s_vTitleConditionType.size() - 1 );
	nLastCount++;
}

void StructTitleManager::RelateTitleCondition( const int nTitleConditionTypeID, TitleCondition * pTitleCondition )
{
	if( !s_hsTitleConditionTypeByID.has( nTitleConditionTypeID ) )
	{
		// TitleCondition이 존재하지 않는 TitleConditionType을 참조
		assert( 0 );
		return;
	}

	size_t idx;
	s_hsTitleConditionTypeByID.lookup( nTitleConditionTypeID, idx );
	s_vTitleConditionType[idx]->vRelatedCondition.push_back( pTitleCondition );
}

TitleBaseServer * StructTitleManager::GetTitleBase( int nCode )
{
	size_t idx;
	if( s_hsTitleID.lookup( nCode, idx ) )
	{
		return s_vTitleBase[idx];
	}

	return NULL;
}

bool StructTitleManager::AchieveTitle( int nCode )
{
	StructTitle *pTitle = setTitleStruct( nCode );

	if( !pTitle )
		return false;

	if( pTitle->GetStatus() & StructTitle::TITLE_STATUS_ACHIEVE )
		return false;

	pTitle->SetStatus( pTitle->GetStatus() | StructTitle::TITLE_STATUS_ACHIEVE );
	m_pHandler->onTitleAchieved( pTitle );

	UpdateTitleConditionByTitleAchieve( pTitle->GetCode() );

	return true;
}

bool StructTitleManager::OpenTitle( int nCode )
{
	StructTitle *pTitle = setTitleStruct( nCode );

	if( !pTitle )
		return false;

	if( pTitle->GetStatus() & ( StructTitle::TITLE_STATUS_OPEN | StructTitle::TITLE_STATUS_ACHIEVE ) )
		return false;

	pTitle->SetStatus( pTitle->GetStatus() | StructTitle::TITLE_STATUS_OPEN );
	m_pHandler->onTitleOpened( pTitle );

	return true;
}

bool StructTitleManager::BookmarkTitle( int nCode )
{
	StructTitle *pTitle = setTitleStruct( nCode );

	if( !pTitle )
		return false;

	pTitle->SetStatus( pTitle->GetStatus() ^ StructTitle::TITLE_STATUS_BOOKMARK );
	pTitle->SetDBUpdateFlag( true );
	m_pHandler->onTitleBookmarked( pTitle );

	return true;
}

bool StructTitleManager::SetTitleChecked( int nCode )
{
	StructTitle *pTitle = setTitleStruct( nCode );

	if( !pTitle )
		return false;

	pTitle->SetStatus( pTitle->GetStatus() );
	pTitle->SetDBUpdateFlag( true );

	return true;
}

void StructTitleManager::CheckTitleCondition( TitleConditionType * & pCondition )
{
	// 변경된 상태에 따라 호칭 획득 가능 여부를 체크한다.
	for( std::vector< TitleCondition * >::const_iterator it = pCondition->vRelatedCondition.begin(); it != pCondition->vRelatedCondition.end(); ++it )
	{
		time_t t = time( NULL );
		TitleBase *pBase = GetTitleBase( (*it)->nTitleID );
		if( pBase->bIsPeriodic && ( pBase->tBeginOfPeriod > t || pBase->tEndOfPeriod < t ) )
			continue;

		bool bOpened = false, bOpen = false, bAchieve = false;
		StructTitle *pTitle = getTitleStruct( (*it)->nTitleID );

		if( pTitle )
		{
			bOpen = bOpened = !!( pTitle->GetStatus() & StructTitle::TITLE_STATUS_OPEN );
			bAchieve = !!( pTitle->GetStatus() & StructTitle::TITLE_STATUS_ACHIEVE );
		}

		// 이미 획득했던 호칭의 조건은 검사하지 않는다.
		if( bAchieve )
			continue;

		if( !bOpened )
			bOpen = checkTitleCondition( (*it)->vTitleOpenConditionInfo );

		if( bOpen )
			bAchieve = checkTitleCondition( (*it)->vTitleAchieveConditionInfo );

		if( bAchieve )
		{
			AchieveTitle( (*it)->nTitleID );
			continue;
		}

		if( !bOpened && bOpen )
		{
			OpenTitle( (*it)->nTitleID );
			continue;
		}
	}
}

void StructTitleManager::CheckTitleConditionByLogin()
{
	for( std::vector< TitleConditionType * >::const_iterator it = s_vTitleConditionType.begin(); it != s_vTitleConditionType.end(); ++it )
	{
		TitleConditionType * pType = (*it);

		CheckTitleCondition( pType );
	}
}

void StructTitleManager::UpdateRelatedTitleCondition( int nTitleConditionType, int nValue1, int nValue2, int nValue3, const __int64 nCount )
{
	// 값이 완전히 동일한 TitleCondition은 단 한 가지 뿐일 것이므로 대상을 찾은 이후 return한다.
	size_t nStart, nEnd;
	GetRelatedTitleConditionRange( nTitleConditionType, nStart, nEnd );
	
	for( size_t i = nStart; i < nEnd; i++ )
	{
		TitleConditionType * & pCondition = s_vTitleConditionType[i];

		if( pCondition->nValue[0] == nValue1 &&
			pCondition->nValue[1] == nValue2 &&
			pCondition->nValue[2] == nValue3 )
		{
			updateTitleCondition( pCondition, nCount, pCondition->bIsSet, pCondition->bSkipDBUpdate );

			return;
		}
	}
}

void StructTitleManager::UpdateTitleCondition( const int nTitleCondition, const __int64 nCount )
{
	size_t nIndex = 0;
	if( !s_hsTitleConditionTypeByID.lookup( nTitleCondition, nIndex ) )
		return;

	TitleConditionType * & pCondition = s_vTitleConditionType[nIndex];
	updateTitleCondition( pCondition, nCount, pCondition->bIsSet, pCondition->bSkipDBUpdate );
}

void StructTitleManager::UpdateTitleConditionByMonsterKill( const int nMonsterCode, const int nTamingCode )
{
	UpdateRelatedTitleCondition( TitleConditionType::TYPE_MONSTER_KILL_BY_CODE, nMonsterCode, 0, 0, 1 );
	UpdateRelatedTitleCondition( TitleConditionType::TYPE_MONSTER_KILL_BY_TAMING_CODE, nTamingCode, 0, 0, 1 );
}

void StructTitleManager::UpdateTitleConditionByItemGet( const ItemBase::ItemCode nItemCode, const __int64 & nCount )
{
	UpdateRelatedTitleCondition( TitleConditionType::TYPE_ITEM_GET_BY_CODE, nItemCode, 0, 0, nCount );
}

void StructTitleManager::UpdateTitleConditionByItemEquip( const ItemBase::ItemCode nItemCode, const bool bEquip )
{
	// 기획 데이터가 존재하지 않아 현재는 사용하지 않는 유형이다.
	UpdateRelatedTitleCondition( TitleConditionType::TYPE_ITEM_EQUIP_BY_CODE, nItemCode, 0, 0, bEquip );
}

void StructTitleManager::UpdateTitleConditionByItemUse( const ItemBase::ItemCode nItemCode )
{
	UpdateRelatedTitleCondition( TitleConditionType::TYPE_ITEM_USE_BY_CODE, nItemCode, 0, 0, 1 );
}

void StructTitleManager::UpdateTitleConditionByItemCreateByMixing( const std::vector< std::pair< StructItem *, __int64 > > & vCreatedItem )
{
	for( std::vector< std::pair< StructItem *, __int64 > >::const_iterator it = vCreatedItem.begin(); it != vCreatedItem.end(); ++it )
	{
		UpdateRelatedTitleCondition( TitleConditionType::TYPE_ITEM_MIX_BY_CODE, (*it).first->GetItemCode(), 0, 0, (*it).second );
	}
}

void StructTitleManager::UpdateTitleConditionBySummonTame( const int nTamingCode, const char nRate, const bool bSuccess )
{
	UpdateRelatedTitleCondition( TitleConditionType::TYPE_SUMMON_TAME_BY_CODE, nTamingCode, bSuccess, 0, 1 );
	UpdateRelatedTitleCondition( TitleConditionType::TYPE_SUMMON_TAME_BY_RATE, nRate, bSuccess, 0, 1 );
}

void StructTitleManager::UpdateTitleConditionBySummonEnhance( const int nTamingCode, const char nRate, const int nEnhance )
{
	size_t nStart, nEnd;
	GetRelatedTitleConditionRange( TitleConditionType::TYPE_SUMMON_ENHANCE_BY_CODE, nStart, nEnd );
	
	for( size_t i = nStart; i < nEnd; i++ )
	{
		TitleConditionType * & pCondition = s_vTitleConditionType[i];

		if( pCondition->nValue[0] == nTamingCode &&
			pCondition->nValue[1] <= nEnhance )
		{
			updateTitleCondition( pCondition, 1, pCondition->bIsSet, pCondition->bSkipDBUpdate );
		}
	}

	GetRelatedTitleConditionRange( TitleConditionType::TYPE_SUMMON_ENHANCE_BY_RATE, nStart, nEnd );
	
	for( size_t i = nStart; i < nEnd; i++ )
	{
		TitleConditionType * & pCondition = s_vTitleConditionType[i];

		if( pCondition->nValue[0] == nRate &&
			pCondition->nValue[1] <= nEnhance )
		{
			updateTitleCondition( pCondition, 1, pCondition->bIsSet, pCondition->bSkipDBUpdate );
		}
	}
}

void StructTitleManager::UpdateTitleConditionBySummonEquip( const int nTamingCode, const char nRate, const int nEnhance, const int nInc )
{
	size_t nStart, nEnd;
	GetRelatedTitleConditionRange( TitleConditionType::TYPE_SUMMON_EQUIP_BY_CODE, nStart, nEnd );
	
	for( size_t i = nStart; i < nEnd; i++ )
	{
		TitleConditionType * & pCondition = s_vTitleConditionType[i];

		if( pCondition->nValue[0] == nTamingCode &&
			pCondition->nValue[1] <= nEnhance )
		{
			updateTitleCondition( pCondition, nInc, pCondition->bIsSet, pCondition->bSkipDBUpdate );
		}
	}

	GetRelatedTitleConditionRange( TitleConditionType::TYPE_SUMMON_EQUIP_BY_RATE, nStart, nEnd );
	
	for( size_t i = nStart; i < nEnd; i++ )
	{
		TitleConditionType * & pCondition = s_vTitleConditionType[i];

		if( pCondition->nValue[0] == nRate &&
			pCondition->nValue[1] <= nEnhance )
		{
			updateTitleCondition( pCondition, nInc, pCondition->bIsSet, pCondition->bSkipDBUpdate );
		}
	}
}

void StructTitleManager::UpdateTitleConditionBySummonMount( const int nTamingCode, const bool bMount )
{
	UpdateRelatedTitleCondition( TitleConditionType::TYPE_SUMMON_MOUNT_BY_CODE, nTamingCode, bMount, 0, 1 );
}

void StructTitleManager::UpdateTitleConditionBySummonPutOnBelt( std::vector< int > & vBeltCode )
{
	int nMaxCount = 0, nCode = 0, nCount = 0;

	std::sort( vBeltCode.begin(), vBeltCode.end() );
	for( std::vector< int >::const_iterator it = vBeltCode.begin(); it != vBeltCode.end(); ++it )
	{
		if( nCode != (*it) )
		{
			if( nCount > nMaxCount )
				nMaxCount = nCount;

			nCode = (*it);
			nCount = 1;
		}
		else
			++nCount;
	}

	if( nCount > nMaxCount )
		nMaxCount = nCount;

	size_t nStart, nEnd;
	GetRelatedTitleConditionRange( TitleConditionType::TYPE_SUMMON_PUT_ON_BELT_WITH_SAME_CODE, nStart, nEnd );
	
	for( size_t i = nStart; i < nEnd; i++ )
	{
		TitleConditionType * & pCondition = s_vTitleConditionType[i];

		if( pCondition->nValue[0] <= nMaxCount )
		{
			updateTitleCondition( pCondition, 1, pCondition->bIsSet, pCondition->bSkipDBUpdate );
		}
	}
}

void StructTitleManager::UpdateTitleConditionBySummonCardGet( const int nTamingCode, const char nRate, const bool bTamedCard, const bool bIsGet )
{
	UpdateRelatedTitleCondition( TitleConditionType::TYPE_ITEM_SUMMON_GET_BY_CODE, nTamingCode, bTamedCard, 0, bIsGet );
	UpdateRelatedTitleCondition( TitleConditionType::TYPE_ITEM_SUMMON_GET_BY_RATE, nRate, bTamedCard, 0, bIsGet );
}

void StructTitleManager::UpdateTitleConditionByQuestStart( const int nQuestCode )
{
	UpdateRelatedTitleCondition( TitleConditionType::TYPE_QUEST_START, nQuestCode, 0, 0, 1 );
}

void StructTitleManager::UpdateTitleConditionByQuestEnd( const int nQuestCode )
{
	UpdateRelatedTitleCondition( TitleConditionType::TYPE_QUEST_END, nQuestCode, 0, 0, 1 );
}

void StructTitleManager::UpdateTitleConditionByDungeonSiegeStart( const int nDungeonID, const bool bIsAttackTeam )
{
	size_t nStart, nEnd;
	GetRelatedTitleConditionRange( TitleConditionType::TYPE_DUNGEON_SIEGE_START, nStart, nEnd );
	
	for( size_t i = nStart; i < nEnd; i++ )
	{
		TitleConditionType * & pCondition = s_vTitleConditionType[i];

		if( ( !pCondition->nValue[0] || pCondition->nValue[0] == nDungeonID ) &&
			!!pCondition->nValue[1] == bIsAttackTeam )
		{
			updateTitleCondition( pCondition, 1, pCondition->bIsSet, pCondition->bSkipDBUpdate );
		}
	}

	UpdateRelatedTitleCondition( TitleConditionType::TYPE_DUNGEON_SIEGE_START, nDungeonID, bIsAttackTeam, 0, 1 );
}

void StructTitleManager::UpdateTitleConditionByDungeonSiegeEnd( const int nDungeonID, const bool bIsAttackTeam, const bool bResult )
{
	size_t nStart, nEnd;
	GetRelatedTitleConditionRange( TitleConditionType::TYPE_DUNGEON_SIEGE_END, nStart, nEnd );
	
	for( size_t i = nStart; i < nEnd; i++ )
	{
		TitleConditionType * & pCondition = s_vTitleConditionType[i];

		if( ( !pCondition->nValue[0] || pCondition->nValue[0] == nDungeonID ) &&
			!!pCondition->nValue[1] == bIsAttackTeam &&
			!!pCondition->nValue[2] == bResult )
		{
			updateTitleCondition( pCondition, 1, pCondition->bIsSet, pCondition->bSkipDBUpdate );
		}
	}
}

void StructTitleManager::UpdateTitleConditionByPKOn( const bool bPKOn )
{
	UpdateRelatedTitleCondition( TitleConditionType::TYPE_CHARACTER_PK_ON, bPKOn, 0, 0, 1 );
}

void StructTitleManager::UpdateTitleConditionByPlayerKill( const c_fixed10 & fImmoralPoint )
{
	size_t nStart, nEnd;
	GetRelatedTitleConditionRange( TitleConditionType::TYPE_CHARACTER_PLAYER_KILL, nStart, nEnd );
	
	for( size_t i = nStart; i < nEnd; i++ )
	{
		TitleConditionType * & pCondition = s_vTitleConditionType[i];

		if( pCondition->nValue[0] <= fImmoralPoint )
		{
			updateTitleCondition( pCondition, 1, pCondition->bIsSet, pCondition->bSkipDBUpdate );
		}
	}
}

void StructTitleManager::UpdateTitleConditionByImmoralPoint( const c_fixed10 & fImmoralPoint )
{
	size_t nStart, nEnd;
	GetRelatedTitleConditionRange( TitleConditionType::TYPE_CHARACTER_IMMORAL_POINT, nStart, nEnd );
	
	for( size_t i = nStart; i < nEnd; i++ )
	{
		TitleConditionType * & pCondition = s_vTitleConditionType[i];

		if( pCondition->nValue[0] <= fImmoralPoint )
		{
			updateTitleCondition( pCondition, 1, pCondition->bIsSet, pCondition->bSkipDBUpdate );
		}
	}
}

void StructTitleManager::UpdateTitleConditionBySkillLevel( const int nSkillID, const int nSkillLevel )
{
	size_t nStart, nEnd;
	GetRelatedTitleConditionRange( TitleConditionType::TYPE_CHARACTER_SKILL, nStart, nEnd );
	
	for( size_t i = nStart; i < nEnd; i++ )
	{
		TitleConditionType * & pCondition = s_vTitleConditionType[i];

		if( pCondition->nValue[0] == nSkillID &&
			pCondition->nValue[1] <= nSkillLevel )
		{
			updateTitleCondition( pCondition, 1, pCondition->bIsSet, pCondition->bSkipDBUpdate );
		}
	}
}

void StructTitleManager::UpdateTitleConditionByTitleAchieve( const int nTitleCode )
{
	UpdateRelatedTitleCondition( TitleConditionType::TYPE_CHARACTER_TITLE, nTitleCode, 0, 0, 1 );
}

void StructTitleManager::UpdateTitleConditionByPCBangMode( const char nPCBangMode )
{
	UpdateRelatedTitleCondition( TitleConditionType::TYPE_CHARACTER_PC_BANG_MODE, nPCBangMode, 0, 0, 1 );
}

void StructTitleManager::UpdateTitleConditionByGold( const StructGold & nGold )
{
	size_t nStart, nEnd;
	GetRelatedTitleConditionRange( TitleConditionType::TYPE_CHARACTER_GOLD, nStart, nEnd );
	
	for( size_t i = nStart; i < nEnd; i++ )
	{
		TitleConditionType * & pCondition = s_vTitleConditionType[i];

		if( pCondition->nValue[0] <= static_cast< int >( ( nGold / 10000 ).GetRawData() ) )
		{
			updateTitleCondition( pCondition, 1, pCondition->bIsSet, pCondition->bSkipDBUpdate );
		}
	}
}

void StructTitleManager::GetRelatedTitleConditionRange( int nTitleConditionType, size_t & nStart, size_t & nEnd )
{
	std::pair< size_t, size_t > * pRange = NULL;
	s_hsTitleConditionTypeByType.lookup( nTitleConditionType, pRange );

	if( !pRange )
	{
		nStart = nEnd = 0;
		return;
	}

	nStart = pRange->first;
	nEnd = pRange->first + pRange->second;
}

const bool StructTitleManager::IsAchievedTitle( int nCode ) const
{
	StructTitle * pTitle = getTitleStruct( nCode );

	if( !pTitle )
		return false;

	return !!( pTitle->GetStatus() & StructTitle::TITLE_STATUS_ACHIEVE );
}

const size_t StructTitleManager::DoEachTitle( struct TitleFunctor & _fo )
{
	size_t nCount = 0;

	for( std::vector< StructTitle * >::iterator it = m_vTitle.begin(); it != m_vTitle.end(); ++it )
	{
		++nCount;
		_fo( *it );
	}

	return nCount;
}

const size_t StructTitleManager::DoEachTitleCondition( struct TitleConditionFunctor & _fo )
{
	size_t nCount = 0;

	for( std::vector< StructTitleCondition * >::iterator it = m_vTitleCondition.begin(); it != m_vTitleCondition.end(); ++it )
	{
		++nCount;
		_fo( *it );
	}

	return nCount;
}

StructTitle * StructTitleManager::getTitleStruct( int nCode ) const
{
	size_t idx;
	if( m_hsTitle.lookup( nCode, idx ) )
	{
		return m_vTitle[idx];
	}

	return NULL;
}

StructTitle * StructTitleManager::setTitleStruct( int nCode )
{
	size_t idx;
	if( m_hsTitle.lookup( nCode, idx ) )
	{
		return m_vTitle[idx];
	}
	else
	{
		// 존재하지 않는 호청
		if( !GetTitleBase( nCode ) )
			return NULL;

		StructTitle * pTitle = StructTitle::AllocTitle( m_pHandler, StructTitleManager::AllocTitleID(), nCode, 0 );
		AddTitle( pTitle );

		pTitle->SetDBInsertFlag();
		return pTitle;
	}
}

StructTitleCondition * StructTitleManager::getTitleCondition( const int nCode ) const
{
	size_t idx;
	if( m_hsTitleCondition.lookup( nCode, idx ) )
	{
		return m_vTitleCondition[idx];
	}

	return NULL;
}

StructTitleCondition * StructTitleManager::setTitleCondition( const int nCode, const __int64 nCount, const bool bIsSet, const bool bSkipDBUpdate, bool & bIsChanged )
{
	StructTitleCondition *pTitleCondition = getTitleCondition( nCode );

	if( pTitleCondition )
	{
		if( bIsSet )
		{
			// 이미 존재하는 동일한 값을 덮어씌우는 경우라면 어떠한 처리도 할 필요가 없다.
			if( pTitleCondition->GetCount() == nCount )
			{
				bIsChanged = false;
				return pTitleCondition;
			}

			pTitleCondition->SetCount( nCount );
		}
		else
		{
			pTitleCondition->AddCount( nCount );
		}

		if( !bSkipDBUpdate )
			pTitleCondition->SetDBUpdateFlag();
	}
	else
	{
		int nSID = !bSkipDBUpdate ? StructTitleManager::AllocTitleConditionID() : 0;

		pTitleCondition = StructTitleCondition::AllocTitleCondition( nSID, nCode, nCount );

		m_vTitleCondition.push_back( pTitleCondition );
		m_hsTitleCondition.add( nCode, m_vTitleCondition.size() - 1 );

		if( !bSkipDBUpdate )
			pTitleCondition->SetDBInsertFlag();
	}

	return pTitleCondition;
}

void StructTitleManager::updateTitleCondition( TitleConditionType * & pCondition, const __int64 nCount, const bool bIsSet, const bool bSkipDBUpdate )
{
	// 저장 및 방송
	bool bIsChanged = true;
	StructTitleCondition * pTitleCondition = setTitleCondition( pCondition->nID, nCount, bIsSet, bSkipDBUpdate, bIsChanged );

	if( !bIsChanged )
		return;

	if( m_pHandler->isReadInfoComplete() )
	{
		m_pHandler->onConditionChanged( pCondition, pTitleCondition->GetCount() );

		CheckTitleCondition( pCondition );
	}
}

const bool StructTitleManager::checkTitleCondition( std::vector< TitleCondition::TITLE_CONDITION_INFO > & vTitleConditionInfo ) const
{
	if( !vTitleConditionInfo.size() )
		return true;

	int nMaxGroupID = 0;
	bool bGroupChecked[ GameRule::TITLE_CONDITION_MAX_GROUP + 1 ];
	memset( bGroupChecked, true, sizeof( bGroupChecked ) );

	std::vector< TitleCondition::TITLE_CONDITION_INFO >::const_iterator it;
	for( it = vTitleConditionInfo.begin(); it != vTitleConditionInfo.end(); ++it )
	{
		// 이미 조건 달성이 불가능한 그룹
		if( !bGroupChecked[ (*it).nGroupID ] )
			continue;

		// 뒤쪽에 몰려있는 달성이 불가능한 그룹은 달성 여부를 확인할 필요없으므로 MaxGroupID에 포함시키지 않는다.
		nMaxGroupID = (*it).nGroupID;

		StructTitleCondition *pTitleCondition = getTitleCondition( (*it).nConditionType );

		if( !pTitleCondition || pTitleCondition->GetCount() < (*it).nCount )
		{
			bGroupChecked[ (*it).nGroupID ] = false;
			continue;
		}
	}

	// 하나라도 만족하는 조건이 있다면 달성
	for( int i = 1; i <= nMaxGroupID; i++ )
	{
		if( bGroupChecked[ i ] )
			return true;
	}

	return false;
}