#pragma once

#include <toolkit/KHash.h>

#include "StructTitle.h"
#include "StructTitleCondition.h"
#include "TitleBase.h"
#include "ItemBase.h"


struct StructTitleManager
{
	struct TitleFunctor
	{
		virtual bool operator()( struct StructTitle * pTitle ) = 0;
	};

	struct TitleConditionFunctor
	{
		virtual bool operator()( struct StructTitleCondition * pTitleCondition ) = 0;
	};

	StructTitleManager();
	virtual ~StructTitleManager();

	void								SetOwner( StructTitle::TitleEventHandler * pHandler ) { m_pHandler = pHandler; }

	void								AddTitle( struct StructTitle * pTitle );
	bool								AddTitleCondition( struct StructTitleCondition * pTitleCondition );

	static void							SetTitleMaxID( int nID );
	static void							SetTitleConditionMaxID( int nID );
	static int							AllocTitleID();
	static int							AllocTitleConditionID();

	static TitleBaseServer *			RegisterTitleBase( TitleBaseServer * base );
	static TitleCondition *				RegisterTitleCondition( TitleCondition * base );
	static void							RegisterTitleConditionType( TitleConditionType * base );
	static void							RelateTitleCondition( const int nTitleConditionTypeID, TitleCondition * pTitleCondition );

	static TitleBaseServer *			GetTitleBase( int nCode );

	bool								AchieveTitle( int nCode );
	bool								OpenTitle( int nCode );
	bool								BookmarkTitle( int nCode );
	bool								SetTitleChecked( int nCode );

	void								CheckTitleCondition( TitleConditionType * & pCondition );
	void								CheckTitleConditionByLogin();
	void								UpdateRelatedTitleCondition( int nTitleConditionType, int nValue1, int nValue2, int nValue3, const __int64 nCount );

	void								UpdateTitleCondition( const int nTitleCondition, const __int64 nCount );
	void								UpdateTitleConditionByMonsterKill( const int nMonsterCode, const int nTamingCode );
	void								UpdateTitleConditionByItemGet( const ItemBase::ItemCode nItemCode, const __int64 & nCount );
	void								UpdateTitleConditionByItemEquip( const ItemBase::ItemCode nItemCode, const bool bEquip );
	void								UpdateTitleConditionByItemUse( const ItemBase::ItemCode nItemCode );
	void								UpdateTitleConditionByItemCreateByMixing( const std::vector< std::pair< struct StructItem *, __int64 > > & vCreatedItem );
	void								UpdateTitleConditionBySummonEnhance( const int nTamingCode, const char nRate, const int nEnhance );
	void								UpdateTitleConditionBySummonTame( const int nTamingCode, const char nRate, const bool bSuccess );
	void								UpdateTitleConditionBySummonEquip( const int nTamingCode, const char nRate, const int nEnhance, const int nInc );
	void								UpdateTitleConditionBySummonMount( const int nTamingCode, const bool bMount );
	void								UpdateTitleConditionBySummonPutOnBelt( std::vector< int > & vBeltSlot );
	void								UpdateTitleConditionBySummonCardGet( const int nTamingCode, const char nRate, const bool bTamedCard, const bool bIsGet );
	void								UpdateTitleConditionByQuestStart( const int nQuestCode );
	void								UpdateTitleConditionByQuestEnd( const int nQuestCode );
	void								UpdateTitleConditionByDungeonSiegeStart( const int nDungeonID, const bool bIsAttackTeam );
	void								UpdateTitleConditionByDungeonSiegeEnd( const int nDungeonID, const bool bIsAttackTeam, const bool bResult );
	void								UpdateTitleConditionByPKOn( const bool bPKOn );
	void								UpdateTitleConditionByPlayerKill( const c_fixed10 & fImmoralPoint );
	void								UpdateTitleConditionByImmoralPoint( const c_fixed10 & fImmoralPoint );
	void								UpdateTitleConditionBySkillLevel( const int nSkillID, const int nSkillLevel );
	void								UpdateTitleConditionByTitleAchieve( const int nTitleCode );
	void								UpdateTitleConditionByPCBangMode( const char nPCBangMode );
	void								UpdateTitleConditionByGold( const StructGold & nGold );

	void								GetRelatedTitleConditionRange( int nTitleConditionType, size_t & nStart, size_t & nEnd );

	const bool							IsAchievedTitle( int nCode ) const;
	const size_t						GetTitleCount()	const			{ return m_vTitle.size(); }
	const size_t						GetTitleConditionCount() const	{ return m_vTitleCondition.size(); }
	const size_t						DoEachTitle( struct TitleFunctor & _fo );
	const size_t						DoEachTitleCondition( struct TitleConditionFunctor & _fo );

protected:
	StructTitle *						getTitleStruct( int nCode ) const;
	StructTitle *						setTitleStruct( int nCode );
	StructTitleCondition *				getTitleCondition( const int nCode ) const;
	StructTitleCondition *				setTitleCondition( const int nCode, const __int64 nCount, const bool bIsSet, const bool bSkipDBUpdate, bool & bIsChanged );

	void								updateTitleCondition( TitleConditionType * & pCondition, const __int64 nCount, const bool bIsSet, const bool bSkipDBUpdate = false );
	const bool							checkTitleCondition( std::vector< TitleCondition::TITLE_CONDITION_INFO > & vTitleConditionInfo ) const;

protected:
	StructTitle::TitleEventHandler *		m_pHandler;

	std::vector< StructTitle * >			m_vTitle;
	KHash< size_t, hashPr_mod_int >			m_hsTitle;
	std::vector< StructTitleCondition * >	m_vTitleCondition;
	KHash< size_t, hashPr_mod_int >			m_hsTitleCondition;
};