#pragma once

#include <ctime>

#include <mmo/ArType.h>
#include <mmo/ArMove.h>


// 대련 타입
enum _COMPETE_TYPE
{
	COMPETE_TYPE_VS_PLAYER		= 0,
	COMPETE_TYPE_VS_PARTY		= 1,
	COMPETE_TYPE_VS_GUILD		= 2,
	COMPETE_TYPE_VS_ALLIANCE	= 3
};
// 대련 요청에 따른 응답 타입(클라이언트의 응답 메시지 및 CompeteManager에 응답 처리 시 사용됨)
enum _COMPETE_ANSWER_TYPE
{
	COMPETE_ANSWER_TYPE_ACCEPT				= 0,	// 수락
	COMPETE_ANSWER_TYPE_REJECT_BY_USER		= 1,	// 거절
	COMPETE_ANSWER_TYPE_REJECT_BY_OPTION	= 2,	// 자동 거절(옵션)
	COMPETE_ANSWER_TYPE_REJECT_BY_TIMEOUT	= 3		// 시간 초과에 따른 취소(신청받은 플레이어에게 보내질 경우 신청 수락/확인 창이 닫히는 것으로 처리)
	// 일정 횟수 이상 연속 거절당함으로 인한 신청 불가 처리는 신청하는 플레이어의 클라이언트에서 처리 함(서버에 요청이 올 일이 없으므로 불필요)
};
// 대련 종료 타입
enum _COMPETE_END_TYPE
{
	COMPETE_END_BY_DEATH				= 0,	// 사망으로 인한 종료
	COMPETE_END_BY_LOGOUT				= 1,	// 로그아웃으로 인한 종료
	COMPETE_END_BY_RINGOUT				= 2,	// 장외로 인한 종료
	COMPETE_END_BY_TIMEOUT				= 3,	// 시간 초과로 인한 종료
	COMPETE_END_BY_INTERRUPT			= 4,	// 몬스터 또는 PK 플레이어의 방해로 인한 종료
	COMPETE_END_BY_ENTERING_SAFETY_ZONE	= 5,	// 대련 불가 지역 진입으로 인한 종료
};
// 대련 상태
enum _COMPETE_STATE
{
	COMPETE_STATE_REQUESTING	= 0,	// 대련 신청 중(신청 시점부터 응답받기 전까지)
	COMPETE_STATE_COUNTDOWN		= 1,	// 대련 시작 전 카운트다운
	COMPETE_STATE_COMPETING		= 2		// 대련 중
};

static const time_t		NOT_SCHEDULED_TIME		= -1;

void assembleCompeteEndMessage( struct TS_SC_COMPETE_END * pMsg, const unsigned char nCompeteType, const unsigned char nEndType, const char * szWinner, const char * szLoser );


// 진행 중인 대련 정보(상속용)
struct CompeteInfoBase
{
	CompeteInfoBase( const _COMPETE_STATE & _eState, const time_t & _tNextStateChange )
		: nID( static_cast< unsigned int >( InterlockedIncrement( &nMaxUsedCompeteID ) ) )
		, eState( _eState )
		, tNextStateChange( _tNextStateChange )
	{}

	virtual const _COMPETE_TYPE		GetCompeteType() const													= 0;

	virtual void					OnAnswerRequest( const _COMPETE_ANSWER_TYPE & eAnswer )					= 0;
	virtual void					OnAnswerTimeout()														= 0;
	virtual void					OnEndCountdown()														= 0;
	virtual void					OnRetire( const void * pRetire, const _COMPETE_END_TYPE & eEndTyper )	= 0;
	virtual void					OnCompeteTimeout()														= 0;

	virtual const unsigned short	Validate( const bool bNotifyEndOnError, const bool bIncludeRequestee = true, const bool bIncludeRequester = true, _COMPETE_END_TYPE * pError = NULL, bool * pbRequesterWin = NULL )	= 0;

	virtual const void *			GetRequestee() const													= 0;
	virtual const void *			GetRequester() const													= 0;
	virtual const char *			GetCompetitorName( const bool bRequester ) const						= 0;

	unsigned int					nID;
	_COMPETE_STATE	eState;
	time_t							tNextStateChange;

private:
	static volatile LONG			nMaxUsedCompeteID;
};


// 개인전 대련 정보
struct PlayerCompeteInfo : public CompeteInfoBase
{
	PlayerCompeteInfo( const AR_HANDLE & _hRequestee, const AR_HANDLE & _hRequester, const _COMPETE_STATE & _eState, const time_t & _tNextStateChange )
		: CompeteInfoBase( _eState, _tNextStateChange )
		, hRequestee( _hRequestee )
		, hRequester( _hRequester )
	{}

	virtual const _COMPETE_TYPE		GetCompeteType() const									{ return COMPETE_TYPE_VS_PLAYER; }

	virtual void					OnAnswerRequest( const _COMPETE_ANSWER_TYPE & eAnswer );
	virtual void					OnAnswerTimeout();
	virtual void					OnEndCountdown();
	virtual void					OnRetire( const void * pRetirer, const _COMPETE_END_TYPE & eEndType );
	virtual void					OnCompeteTimeout();

	virtual const unsigned short	Validate( const bool bNotifyEndOnError, const bool bIncludeRequestee = true, const bool bIncludeRequester = true, _COMPETE_END_TYPE * peError = NULL, bool * pbRequesterWin = NULL );

	virtual const void *			GetRequestee() const									{ return &hRequestee; }
	virtual const void *			GetRequester() const									{ return &hRequester; }
	virtual const char *			GetCompetitorName( const bool bRequester ) const;

	inline const struct StructPlayer *		GetRequesteeAsPlayer() const;
	inline struct StructPlayer *			GetRequesteeAsPlayer();
	inline const struct StructPlayer *		GetRequesterAsPlayer() const;
	inline struct StructPlayer *			GetRequesterAsPlayer();

	AR_HANDLE	hRequestee;
	AR_HANDLE	hRequester;

	ArPosition	posCompeteCenter;
};

// 파티전 대련 정보
struct PartyCompeteInfo : public CompeteInfoBase
{
	PartyCompeteInfo( const int & _nRequestee, const int & _nRequester, const _COMPETE_STATE & _eState, const time_t & _tNextStateChange )
		: CompeteInfoBase( _eState, _tNextStateChange )
		, nRequestee( _nRequestee )
		, nRequester( _nRequester )
	{}

	virtual const _COMPETE_TYPE		GetCompeteType() const									{ return COMPETE_TYPE_VS_PARTY; }

	virtual void					OnAnswerRequest( const _COMPETE_ANSWER_TYPE & eAnswer );
	virtual void					OnAnswerTimeout();
	virtual void					OnEndCountdown();
	virtual void					OnRetire( const void * pRetirer, const _COMPETE_END_TYPE & eEndType );
	virtual void					OnCompeteTimeout();

	virtual const unsigned short	Validate( const bool bNotifyEndOnError, const bool bIncludeRequestee = true, const bool bIncludeRequester = true, _COMPETE_END_TYPE * pError = NULL, bool * pbRequesterWin = NULL );

	virtual const void *			GetRequestee() const									{ return &nRequestee; }
	virtual const void *			GetRequester() const									{ return &nRequester; }
	virtual const char *			GetCompetitorName( const bool bRequester ) const;

	int			nRequestee;
	int			nRequester;
};

// 길드전 대련 정보
struct GuildCompeteInfo : public CompeteInfoBase
{
	GuildCompeteInfo( const int & _nRequestee, const int & _nRequester, const _COMPETE_STATE & _eState, const time_t & _tNextStateChange )
		: CompeteInfoBase( _eState, _tNextStateChange )
		, nRequestee( _nRequestee )
		, nRequester( _nRequester )
	{}

	virtual const _COMPETE_TYPE		GetCompeteType() const									{ return COMPETE_TYPE_VS_GUILD; }

	virtual void					OnAnswerRequest( const _COMPETE_ANSWER_TYPE & eAnswer );
	virtual void					OnAnswerTimeout();
	virtual void					OnEndCountdown();
	virtual void					OnRetire( const void * pRetirer, const _COMPETE_END_TYPE & eEndType );
	virtual void					OnCompeteTimeout();

	virtual const unsigned short	Validate( const bool bNotifyEndOnError, const bool bIncludeRequestee = true, const bool bIncludeRequester = true, _COMPETE_END_TYPE * pError = NULL, bool * pbRequesterWin = NULL );

	virtual const void *			GetRequestee() const									{ return &nRequestee; }
	virtual const void *			GetRequester() const									{ return &nRequester; }
	virtual const char *			GetCompetitorName( const bool bRequester ) const;

	int			nRequestee;
	int			nRequester;
};

// 연합전 대련 정보
struct AllianceCompeteInfo : public CompeteInfoBase
{
	AllianceCompeteInfo( const int & _nRequestee, const int & _nRequester, const _COMPETE_STATE & _eState, const time_t & _tNextStateChange )
		: CompeteInfoBase( _eState, _tNextStateChange )
		, nRequestee( _nRequestee )
		, nRequester( _nRequester )
	{}

	virtual const _COMPETE_TYPE		GetCompeteType() const									{ return COMPETE_TYPE_VS_ALLIANCE; }

	virtual void					OnAnswerRequest( const _COMPETE_ANSWER_TYPE & eAnswer );
	virtual void					OnAnswerTimeout();
	virtual void					OnEndCountdown();
	virtual void					OnRetire( const void * pRetirer, const _COMPETE_END_TYPE & eEndType );
	virtual void					OnCompeteTimeout();

	virtual const unsigned short	Validate( const bool bNotifyEndOnError, const bool bIncludeRequestee = true, const bool bIncludeRequester = true, _COMPETE_END_TYPE * pError = NULL, bool * pbRequesterWin = NULL );

	virtual const void *			GetRequestee() const									{ return &nRequestee; }
	virtual const void *			GetRequester() const									{ return &nRequester; }
	virtual const char *			GetCompetitorName( const bool bRequester ) const;

	int			nRequestee;
	int			nRequester;
};
