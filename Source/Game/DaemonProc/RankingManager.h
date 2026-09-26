#pragma once

#include <vector>
#include <string>

#include <toolkit/ILock.h>
#include <toolkit/XSTLUtil.h>
#include <mmo/ArSchedulerObject.h>
#include <toolkit/KHash.h>
#include <toolkit/c_fixed.h>

#include "GameDBManager.h"


struct RankingManager : public ArSchedulerObject
{
public:
	enum _RANKING_TYPE
	{
		RANKING_TYPE_DONATION			= 0,	// 기부 포인트 랭킹
		RANKING_TYPE_DONATION_REWARD,			// 기부 랭킹에 의해 선정된 지난 달 최고 랭커 목록(스코어에 따른 랭킹 대상이 아님)
		RANKING_TYPE_HUNTAHOLIC_THIS_MONTH,		// 헌터홀릭 이번 달 랭킹
		RANKING_TYPE_HUNTAHOLIC_LAST_MONTH,		// 헌터홀릭 지난 달 랭킹
		RANKING_TYPE_HUNTAHOLIC_TOTAL,			// 헌터홀릭 현재까지 누적 랭킹

		RANKING_TYPE_MAX_INDEX					// 랭킹 타입 최대 수
	};

protected:
	static const unsigned int	RANK_UNKNOWN											= 0xFFFFFFFF;	// 랭킹 미평가 상태를 나타내는 랭킹 값 상수
	static const size_t			RANKING_TOP_RECORD_MIN_RANK[ RANKING_TYPE_MAX_INDEX ];					// 랭킹 타입별 상위 랭킹 레코드에 들기 위한 최소 등수(UI에 표시되는 항목 수)
	static const c_fixed10		RANKING_TOP_RECORD_MIN_SCORE[ RANKING_TYPE_MAX_INDEX ];					// 랭킹 타입별 상위 랭킹 레코드에 들기 위한 최소 점수

	struct _RANKING_DATA
	{
		_RANKING_DATA( const int nPlayerUID, const c_fixed10 & fScore, const bool bIsValid )
			: m_nPlayerUID( nPlayerUID )
			, m_fScore( fScore )
			, m_nRank( RANK_UNKNOWN )
			, m_bIsValid( bIsValid )
		{}

		const int		m_nPlayerUID;
		c_fixed10		m_fScore;
		unsigned int	m_nRank;
		bool			m_bIsValid;
	};

	struct _RANKING_INFO
	{
	public:
		_RANKING_INFO()
			: m_tNextSettling( -1 )
		{}

		XCriticalSection &	GetLock() const			{ return m_csRanking; }

		const unsigned int	GetRank( const int nPlayerUID ) const;
		const c_fixed10		GetRankingScore( const int nPlayerUID ) const;
		void				SetRankingScore( const int nPlayerUID, const char * pszPlayerName, const c_fixed10 & fScore, const bool bIsValid );
		void				ClearRankingData();
	protected:
		// 특정 랭킹 데이터의 순위를 다시 체크
		void				rerankData( _RANKING_DATA * pRank, const bool bIsScoreUp );

	public:
		mutable XCriticalSection					m_csRanking;		// 이하 2개의 컨테이너와 m_tNextSettling을 보호
		std::vector< _RANKING_DATA * >				m_vRanking;			// 순위별 정렬을 위한 컨테이너
		KHash< _RANKING_DATA *, hashPr_mod_int >	m_hsRanking;		// PlayerUID 기반 임의 접근을 위한 컨테이너
		time_t										m_tNextSettling;	// 주기별 초기화 처리 예정 시각
	};

protected:
	RankingManager() : m_bInitialized( false )		{}
public:
	static RankingManager & Instance();
	virtual ~RankingManager();

	void				Init();
	void				DeInit();
	const bool			IsInitialized() const;

	void				Push( GameDBManager::DBProc* pWork );
	void				onEndQuery();

	const time_t		GetNextSettlingTime( const _RANKING_TYPE & eType ) const;
	const bool			SetNextSettlingTime( const _RANKING_TYPE & eType, const time_t & tNextSettlingTime );

	const unsigned int	GetRank( const _RANKING_TYPE & eType, const int nPlayerUID ) const;
	const c_fixed10		GetRankingScore( const _RANKING_TYPE & eType, const int nPlayerUID ) const;
	void				SetRankingScore( const _RANKING_TYPE & eType, const int nPlayerUID, const char * pszPlayerName, const c_fixed10 & fScore, const bool bIsValid );
	void				AddRankingScore( const _RANKING_TYPE & eType, const int nPlayerUID, const char * pszPlayerName, const c_fixed10 & fAddend, const bool bIsValid );

	void				DeleteRankingScore( const int nPlayerUID );

	typedef	std::pair< int /* PlayerUID */, c_fixed10 /* score */ > RankingRecord;
	void				GetRankingTopRecord( const _RANKING_TYPE & eType, std::vector< RankingRecord > & vTopRecord );

	const bool			IsValidRankingScore( const _RANKING_TYPE & eType, const int nPlayerUID ) const;
	const bool			InvalidateRankingScore( const _RANKING_TYPE & eType, const int nPlayerUID );

	virtual void		onProcess( int nThreadIdx );

protected:
	_RANKING_INFO *			getRankingInfo( const _RANKING_TYPE & eType )
	{
		assert( eType >= RANKING_TYPE_DONATION && eType < RANKING_TYPE_MAX_INDEX );
		return ( eType < 0 || eType >= RANKING_TYPE_MAX_INDEX ) ? NULL : ( m_aRankingInfo + eType );
	}
	const _RANKING_INFO *	getRankingInfo( const _RANKING_TYPE & eType ) const
	{
		assert( eType >= RANKING_TYPE_DONATION && eType < RANKING_TYPE_MAX_INDEX );
		return ( eType < 0 || eType >= RANKING_TYPE_MAX_INDEX ) ? NULL : ( m_aRankingInfo + eType );
	}

private:
	bool									m_bInitialized;

	_RANKING_INFO							m_aRankingInfo[ RANKING_TYPE_MAX_INDEX ];

	XCriticalSection						m_QueryLock;
	std::list< GameDBManager::DBProc* >		m_lQueryList;
};
