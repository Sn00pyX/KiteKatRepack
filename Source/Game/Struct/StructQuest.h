#pragma once
#include "QuestBase.h"
#include "QuestInstance.h"

struct StructQuest
{
	enum
	{
		QUEST_CODE_CHAOS_COLLECT = 1032,
	};

	struct QuestEventHandler
	{
		virtual void onStatusChanged( struct StructQuest *pQuest ) = 0;
		virtual void onProgressChanged( struct StructQuest *pQuest, QuestInstance::QUEST_PROGRESS oldProgress, QuestInstance::QUEST_PROGRESS newProgress ) = 0;
	};

	static StructQuest *	AllocQuest( struct QuestEventHandler *pHandler, 
								int nId,
								QuestBase::QuestCode code,
								int status[],
								const AR_TIME nTimeLimit,
								QuestInstance::QUEST_PROGRESS progress,
								int nStartID );
	void					FreeQuest();

	StructQuest(	struct QuestEventHandler *pHandler, 
					int nId,
					QuestBase::QuestCode code,
					int status[],
					const AR_TIME nTimeLimit,
					QuestInstance::QUEST_PROGRESS progress,
					int nStartID );

	virtual ~StructQuest();

	struct QuestEventHandler *	GetHandler() const									{ return m_pHandler; }
	void						SetHandler( struct QuestEventHandler *pHandler )	{ m_pHandler = pHandler; }

	static bool RegisterQuestBase( const QuestBaseServer & base );

	inline QuestBase::QuestCode		GetQuestCode() const		{ return m_Instance.Code;		}
	inline const QuestBaseServer &	GetQuestBase() const		{ return *m_pQuestBase;			}
	inline const QuestInstance &	GetQuestInstance() const	{ return m_Instance;			}
	inline QuestInstance::QuestID	GetQuestID() const			{ return m_Instance.nId;		}
		
	inline QuestBase::QuestType					GetQuestType() const						{ return GetQuestBase().nType;	}
	inline bool									IsRepeatable() const						{ return GetQuestBase().bIsRepeatable; }

	inline const QuestBase::TIME_LIMIT_TYPE		GetTimeLimitType() const					{ return GetQuestBase().eTimeLimitType; }
	const AR_TIME								GetTimeLimit() const						{ return m_Instance.nTimeLimit; }
	const int									GetRemainTime() const;
	void										SetTimeLimit( const AR_TIME & nTimeLimit );

	inline QuestInstance::QUEST_PROGRESS		GetProgress() const	{ return m_Instance.nProgress; }
	inline void									SetProgress( QuestInstance::QUEST_PROGRESS progress )
	{
		QuestInstance::QUEST_PROGRESS oldProgress = m_Instance.nProgress;
		m_Instance.nProgress = progress;
		if( m_pHandler ) m_pHandler->onProgressChanged( this, oldProgress, progress );
		TurnOnUpdateFlag();
	}

	int							GetValue( int idx )  const;
	int							GetStatus( int idx )  const;
	void						UpdateStatus( int idx, int value );
	void						IncStatus( int idx, int value = 1 );
	void						ResetStatus();
	
	int							GetRandomKey( int idx ) const;
	int							GetRandomValue( int idx ) const;
	void						SetRandomKey( int idx, int value );
	void						SetRandomValue( int idx, int value );

	static const QuestBaseServer &	GetQuestBase( QuestBase::QuestCode c );

	bool						IsFinishable()  const;
	inline bool					IsRandomQuest()	const		{ return IsRandomQuest( GetQuestCode() ); }
	static bool					IsRandomQuest( int code );

	// 보상 관련
	int							GetRewardFavorId();
	int							GetRewardHateId();

	int							GetFavor() const			{ return GetQuestBase().nFavor; }

	__int64						GetExp()  const				{ return GetQuestBase().nExp; }
	int							GetJP()	 const				{ return GetQuestBase().nJP; }
	int							GetHuntaholicPoint() const	{ return GetQuestBase().nHuntaholicPoint; }
	const StructGold			GetGold()  const			{ return GetQuestBase().nGold; }

	const QuestBase::Reward &	GetDefaultReward() const	{ return GetQuestBase().DefaultReward; }
	const QuestBase::Reward &	GetOptionalReward( int idx ) const;


	// 구현용 인터페이스
	inline bool					IsNeedUpdateToDB()			{ return m_bIsNeedUpdateToDB;	}
	inline void					TurnOffDbUpdateFlag()		{ m_bIsNeedUpdateToDB = false; }
	inline void					TurnOnUpdateFlag()			{ m_bIsNeedUpdateToDB = true;	}

protected:
	const QuestBaseServer*		m_pQuestBase;
	struct QuestEventHandler*	m_pHandler;
	QuestInstance				m_Instance;
	bool						m_bIsNeedUpdateToDB;
};