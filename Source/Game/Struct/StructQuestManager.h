#pragma once

#include <vector>
#include <toolkit/KHash.h>

#include "StructQuest.h"

struct StructQuestManager
{
	enum
	{
		QUEST_MAX	= 20,
	};

	struct QuestFunctor
	{
		virtual bool operator()( struct StructQuest * pQuest ) = 0;
	};

	StructQuestManager();
	virtual ~StructQuestManager();

	void					SetOwner( struct StructQuest::QuestEventHandler* pHandler ) { m_pHandler = pHandler; }

	const size_t			DoEachActiveQuest( struct QuestFunctor & _fo );
	const size_t			DoEachPendingQuest( struct QuestFunctor & _fo );

	void					SetMaxQuestID( QuestInstance::QuestID id );

	bool					AddQuest( struct StructQuest* pQuest );
	void					AddRandomQuestInfo( int code, int key[], int value[], bool is_dropped );
	void					ReadQuestCoolTime( int code, AR_TIME cool_time, QuestInstance::QUEST_PROGRESS progress );
	void					AddQuestCoolTime( int code, AR_TIME cool_time, QuestInstance::QUEST_PROGRESS progress );
	void					RemoveQuestCoolTime( int code );
	const time_t			GetQuestCoolTime( int code ) const;
	const time_t			GetRemainQuestCoolTime( int code, time_t tCurrent ) const;

	const bool				AddPendingQuest( const QuestBase::QuestCode code, const int nStartID );
	bool					StartQuest( QuestBase::QuestCode code, int nStartID );
	bool					EndQuest( struct StructQuest* pQuest );
	const bool				ProcExpiredQuest( const AR_TIME t );

	struct StructQuest*		FindQuest( QuestBase::QuestCode code ) const;

	const bool				IsPendingQuest( const QuestBase::QuestCode code ) const;
	bool					IsFinishedQuest( QuestBase::QuestCode code ) const;
	QuestInstance::QuestID	GetFinishedQuestID( QuestBase::QuestCode code ) const;
	const bool				ResetFinishedQuest( QuestBase::QuestCode code );

	bool					IsTakeableQuestItem( QuestBase::QuestCode code ) const;

	void					GetRelatedQuest( std::vector< StructQuest* > & vQuestList, int flag = QuestBase::TYPE_FLAG_ALL );
	void					GetRelatedQuestByItem( ItemBase::ItemCode code, std::vector< StructQuest* > & vQuestList, int flag = QuestBase::TYPE_FLAG_ALL );
	void					GetRelatedQuestByMonster( int nMonsterId, std::vector< StructQuest* > & vQuestList, int flag = QuestBase::TYPE_FLAG_ALL );

	void					UpdateQuestStatusByItemCount( ItemBase::ItemCode code, const __int64 & count );
	void					UpdateQuestStatusByPlayerKill( const struct StructPlayer * pKilledPlayer );
	void					UpdateQuestStatusByMonsterKill( int nMonsterId );
	void					UpdateQuestStatusBySkillLevel( int nSkillId, int nSkillLevel );
	void					UpdateQuestStatusByJobLevel( int nJobDepth, int nJobLevel );
	void					UpdateQuestStatusByParameter( int parameter_id, int value );
	void					UpdateQuestStatusByItemEnhance( struct StructInventory* inventory, const ItemBase::ItemCode code = 0 );


	void					PopFromActiveQuest( struct StructQuest * pQuest );
	// 시작 대기 중인 퀘스트도 활성화 퀘스트 수에 포함됨
	inline const size_t		GetActiveQuestCount() const	{ return m_vActiveQuest.size(); }

	bool					IsStartableQuest( const QuestBase::QuestCode code ) const;

	void					SetDropFlagToRandomQuestInfo( int code );
	bool					HasRandomQuestInfo( int code ) const;
	bool					NeedToUpdateRandomQuestInfo( int code ) const;

protected:

	struct StructQuest::QuestEventHandler*		m_pHandler;
	QuestInstance::QuestID			allocQuestID();

	KHash< QuestInstance::QuestID, hashPr_mod_int >	m_hsFinishedQuest;	// 완료한 퀘스트
	std::vector< StructQuest * >	m_vActiveQuest;			// 수행중인 QuestList

	volatile QuestInstance::QuestID			m_QuestIdx;

	struct RandomQuestInfo
	{
		RandomQuestInfo( int _code ) : code( _code )
		{
			for( int i = 0; i < 3; ++i )
				key[i] = value[i] = 0;

			is_dropped = false;
		}

		int		code;

		int		key[QuestInstance::MAX_RANDOM_VALUE];
		int		value[QuestInstance::MAX_RANDOM_VALUE];

		bool	is_dropped;
	};

	std::vector< RandomQuestInfo >	m_vRandomQuestInfo;

	struct QuestCoolTimeInfo
	{
		QuestCoolTimeInfo()	{}
		QuestCoolTimeInfo( time_t _cool_time, QuestInstance::QUEST_PROGRESS _progress )	: cool_time( _cool_time ), progress( _progress )	{}

		time_t							cool_time;
		QuestInstance::QUEST_PROGRESS	progress;
	};

	KHash< QuestCoolTimeInfo *, hashPr_mod_int >	m_hsQuestCoolTime;	// 퀘스트 쿨타임을 위한 정보
};