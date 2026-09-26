
#include "BattleArena.h"


BattleArena::BattleArena( const BattleArenaBaseServer* pArenaBase, ThreadSafeIntMap* pPartyToArenaMap )
	: m_pArenaBase( pArenaBase )
	, m_pPartyToArenaMap( pPartyToArenaMap )
	, m_csArena( "BattleArena::csArena" )
	, m_nLeastFreeInstanceSlot( 1 )
{
	// MAX_BATTLE_ARENA_INSTANCE_NO_PER_ARENA 값 오버플로우 체크(static_assert 같은 게 있으면 좀 더 좋았을듯)
	assert( MAX_BATTLE_ARENA_INSTANCE_NO_PER_ARENA > 0 && MAX_BATTLE_ARENA_INSTANCE_NO_PER_ARENA < 255 );

	for( int i = 0; i < _countof( m_aInstanceNoUseFlag ); ++i )
	{
		m_aInstanceNoUseFlag[i] = false;
	}

	for( int i = 0; i < _countof( m_apBattleList ); ++i )
	{
		m_apBattleList[i] = NULL;
	}
}

BattleArena::~BattleArena()
{
	for( int i = 0 ; i < _countof( m_apBattleList ) ; ++i )
	{
		BattleArenaInstance* pInstance = m_apBattleList[ i ];
		if( pInstance == NULL )
			continue;

		delete pInstance;
	}

	m_vActiveLayerList.clear();

	for( int i = 0; i < _countof( m_aInstanceNoUseFlag ); ++i )
	{
		m_aInstanceNoUseFlag[i] = false;
	}

	for( int i = 0; i < _countof( m_apBattleList ); ++i )
	{
		m_apBattleList[i] = NULL;
	}

	// 아레나에 등록되어 있던 BattleArenaBaseServer 구조체도 메모리 해제
	for( std::vector< const BATTLE_ARENA_MONSTER_RESPAWN* >::const_iterator it = m_pArenaBase->vMonsterRespawnList.begin();
		it != m_pArenaBase->vMonsterRespawnList.end();
		++it )
	{
		delete (*it);
	}

	const_cast< BattleArenaBaseServer* >( m_pArenaBase )->vMonsterRespawnList.clear();

	for( std::vector< const BATTLE_ARENA_FIELD_PROP_RESPAWN_INFO* >::const_iterator it = m_pArenaBase->vFieldPropRespawnList.begin();
		it != m_pArenaBase->vFieldPropRespawnList.end();
		++it )
	{
		delete (*it);
	}

	const_cast< BattleArenaBaseServer* >( m_pArenaBase )->vFieldPropRespawnList.clear();

	for( std::vector< const BATTLE_ARENA_FIELD_PROP_RESPAWN_INFO* >::const_iterator it = m_pArenaBase->vArenaBlockerFieldPropRespawnList.begin();
		it != m_pArenaBase->vArenaBlockerFieldPropRespawnList.end();
		++it )
	{
		delete (*it);
	}

	const_cast< BattleArenaBaseServer* >( m_pArenaBase )->vArenaBlockerFieldPropRespawnList.clear();

	for( int nTeamNo = 0 ; nTeamNo < m_pArenaBase->nTeamCount ; ++nTeamNo )
	{
		const X2D::Polygon< AR_UNIT >* pPolygon = m_pArenaBase->apBeginAreaList[ nTeamNo ];
		if( pPolygon == NULL )
			continue;

		delete pPolygon;
		const_cast< X2D::Polygon< AR_UNIT >*& >( m_pArenaBase->apBeginAreaList[ nTeamNo ] ) = NULL;
	}

	delete m_pArenaBase;
}

bool	BattleArena::_isOccupiedInstanceNo( unsigned char nInstanceNo ) const
{
	if( IsValidBattleArenaInstanceNo( nInstanceNo ) )
	{
		return m_aInstanceNoUseFlag[ nInstanceNo ];
	}

	return false;
}

ArcadiaLock	BattleArena::LockWholeArena( unsigned short nLayer ) const
{
	// 락 체크(지역 락 -> csArena 순서이므로 csArena가 안 걸려있어야 함)
	assert( !m_csArena.IsLockedByCurrentThread() );

	return _LockWholeBattleArena( m_pArenaBase, nLayer );
}

unsigned char	BattleArena::_allocNewInstanceNo()
{
	// 락 체크
	assert( m_csArena.IsLockedByCurrentThread() );

	if( m_nLeastFreeInstanceSlot == INVALID_BATTLE_ARENA_INSTANCE_NO )
		return INVALID_BATTLE_ARENA_INSTANCE_NO;

	unsigned char nAllocatedInstanceNo = m_nLeastFreeInstanceSlot;
	assert( IsValidBattleArenaInstanceNo( nAllocatedInstanceNo ) );
	if( IsValidBattleArenaInstanceNo( nAllocatedInstanceNo ) )
	{
		m_aInstanceNoUseFlag[ nAllocatedInstanceNo ] = true;

		// nLeastFreeInstanceSlot 갱신
		while( ++m_nLeastFreeInstanceSlot <= MAX_BATTLE_ARENA_INSTANCE_NO_PER_ARENA )
		{
			if( !m_aInstanceNoUseFlag[ m_nLeastFreeInstanceSlot ] )
				break;
		}

		if( m_nLeastFreeInstanceSlot > MAX_BATTLE_ARENA_INSTANCE_NO_PER_ARENA )
			m_nLeastFreeInstanceSlot = INVALID_BATTLE_ARENA_INSTANCE_NO;

		m_vActiveLayerList.push_back( nAllocatedInstanceNo );
	}

	return nAllocatedInstanceNo;
}

void	BattleArena::_freeInstanceNo( unsigned char nInstanceNo )
{
	// 락 체크
	assert( m_csArena.IsLockedByCurrentThread() );

	assert( IsValidBattleArenaInstanceNo( nInstanceNo ) );

	// BattleArenaInstance에 할당되었던 nInstanceNo인지 체크
	assert( m_aInstanceNoUseFlag[ nInstanceNo ] && std::find( m_vActiveLayerList.begin(), m_vActiveLayerList.end(), nInstanceNo ) != m_vActiveLayerList.end() );

	if( IsValidBattleArenaInstanceNo( nInstanceNo ) )
	{
		m_aInstanceNoUseFlag[ nInstanceNo ] = false;

		m_vActiveLayerList.erase( std::find( m_vActiveLayerList.begin(), m_vActiveLayerList.end(), nInstanceNo ) );

		// nLeastFreeInstanceSlot 갱신
		if( nInstanceNo < m_nLeastFreeInstanceSlot )
			m_nLeastFreeInstanceSlot = nInstanceNo;
	}
}

bool	BattleArena::_isFull() const
{
	// 락 체크
	assert( m_csArena.IsLockedByCurrentThread() );

	// _allocNewInstanceNo 함수 구현 참조
	return m_nLeastFreeInstanceSlot == INVALID_BATTLE_ARENA_INSTANCE_NO;
}

BattleArenaInstance*	BattleArena::_getBattleInstance( unsigned char nInstanceNo )
{
	// 락 체크
	assert( m_csArena.IsLockedByCurrentThread() );

	if( !IsValidBattleArenaInstanceNo( nInstanceNo ) )
	{
		assert( 0 );
		return NULL;
	}

	if( !_isOccupiedInstanceNo( nInstanceNo ) )
		return NULL;

	return m_apBattleList[ nInstanceNo ];
}

const BattleArenaInstance*	BattleArena::_getBattleInstance( unsigned char nInstanceNo ) const
{
	// 락 체크
	assert( m_csArena.IsLockedByCurrentThread() );

	if( !IsValidBattleArenaInstanceNo( nInstanceNo ) )
	{
		assert( 0 );
		return NULL;
	}

	if( !_isOccupiedInstanceNo( nInstanceNo ) )
		return NULL;

	return m_apBattleList[ nInstanceNo ];
}
