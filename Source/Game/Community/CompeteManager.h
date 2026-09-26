#pragma once

#include <toolkit/ILock.h>
#include <mmo/ArSchedulerObject.h>

#include "CompeteBase.h"
#include "StructPlayer.h"


struct CompeteManager : public ArSchedulerObject
{
protected:
	CompeteManager()		{}
public:
	static CompeteManager & Instance()
	{
		static CompeteManager _instance;
		return _instance;
	}
	~CompeteManager();

	void					Init();
	void					DeInit();

public:
	const unsigned short			RequestCompeteToPlayer( struct StructPlayer * pRequester, struct StructPlayer * pRequestee );
	const unsigned short			AnswerRequestFromPlayer( struct StructPlayer * pRequestee, const _COMPETE_ANSWER_TYPE & eAnswerType );
	const unsigned short			RetireCompeteWithPlayer( struct StructPlayer * pRetirer, const _COMPETE_END_TYPE & eEndType );

	//TODO: 대련 플레이어나 주변 지역에 방송을 해야하는 경우에 지역락을 걸 영역을 체크하기 위해 사용해야 함(주변 방송이 없으면 쓸 필요 없음)
	template< typename T >
	const T							GetCompetitor( const _COMPETE_TYPE & eType, const T & lhs ) const
	{
		for( std::vector< CompeteInfoBase * >::const_iterator it = m_vCompeteInfo.begin() ; it != m_vCompeteInfo.end() ; ++it )
		{
			if( (*it)->GetCompeteType() != eType )
				continue;

			if( *static_cast< const T * >( (*it)->GetRequestee() ) == lhs )
				return *static_cast< const T * >( (*it)->GetRequester() );
			if( *static_cast< const T * >( (*it)->GetRequester() ) == lhs )
				return *static_cast< const T * >( (*it)->GetRequestee() );
		}

		return NULL;
	}

	typedef StructPlayer * PlayerPtr;
	template<>
	const PlayerPtr					GetCompetitor< PlayerPtr >( const _COMPETE_TYPE & eType, const PlayerPtr & pPlayer ) const
	{
		if( eType != COMPETE_TYPE_VS_PLAYER )
		{
			assert( 0 );
			return NULL;
		}

		AR_HANDLE hPlayer = pPlayer->GetHandle();
		for( std::vector< CompeteInfoBase * >::const_iterator it = m_vCompeteInfo.begin() ; it != m_vCompeteInfo.end() ; ++it )
		{
			if( (*it)->GetCompeteType() != eType )
				continue;

			if( *static_cast< const AR_HANDLE * >( (*it)->GetRequestee() ) == hPlayer )
				return *StructPlayer::get( *static_cast< const AR_HANDLE * >( (*it)->GetRequester() ) );
			if( *static_cast< const AR_HANDLE * >( (*it)->GetRequester() ) == hPlayer )
				return *StructPlayer::get( *static_cast< const AR_HANDLE * >( (*it)->GetRequestee() ) );
		}

		return NULL;
	}

	const bool						IsStarted( const unsigned int nCompeteID, const bool bIncludeCounting ) const;

	virtual void					onProcess( int nThreadIdx );

protected:
	// 타입에 맞는 CompeteInfo를 리스트에 추가하고 ID값을 생성하여 리턴
	const unsigned int		insertCompeteInfo( const _COMPETE_TYPE & eType, const void * pRequestee, const void * pRequester, const _COMPETE_STATE & eState, const time_t & tNextStateChange );
	// CompeteInfo를 삭제(메모리에서 delete됨)
	const bool				deleteCompeteInfo( const unsigned int nCompeteID );

	CompeteInfoBase *		getCompeteInfo( const unsigned int nCompeteID ) const;
	inline const char *		getCompetitorName( const CompeteInfoBase * pCompeteInfo, const bool bRequester ) const;

	const bool				validateCompeteInfo( CompeteInfoBase * pCompeteInfo );

protected:
	// m_vCompeteInfo 벡터와 StructPlayer::nCompeteID를 보호(지역 락 -> m_csCompete 순서여야 함)
	mutable XCriticalSection			m_csCompete;
	std::vector< CompeteInfoBase * >	m_vCompeteInfo;
};
