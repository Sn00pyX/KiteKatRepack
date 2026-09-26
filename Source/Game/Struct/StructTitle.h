#pragma once

#ifdef _MEM_USAGE_DEBUG
#include <dump/XExceptionHandler.h>
#endif


struct StructTitle
{
	enum
	{
		TITLE_STATUS_OPEN			= ( 1 << 0 ),
		TITLE_STATUS_ACHIEVE		= ( 1 << 1 ),
		TITLE_STATUS_BOOKMARK		= ( 1 << 2 ),
	};

	struct TitleEventHandler
	{
		virtual bool isReadInfoComplete() = 0;
		virtual void onTitleAchieved( StructTitle * pTitle ) = 0;
		virtual void onTitleOpened( StructTitle * pTitle ) = 0;
		virtual void onTitleBookmarked( StructTitle * pTitle ) = 0;
		virtual void onConditionChanged( struct TitleConditionType * pCondition, const __int64 nCount ) = 0;
	};

	static StructTitle *	AllocTitle( TitleEventHandler *pHandler, int nSID, int nCode, int nStatus );
	void					FreeTitle();

	StructTitle( struct TitleEventHandler *pHandler, int nSID, int nCode, int nStatus )
		: m_pHandler( pHandler ), m_nSID( nSID ), m_nCode( nCode ), m_nStatus( nStatus ), m_bDBInsertFlag( false ), m_bDBUpdateFlag( false )
	{
#ifdef _MEM_USAGE_DEBUG
		XSEH::IncreaseAllocCount( "StructTitle" );
#endif
	}

	~StructTitle()
	{
#ifdef _MEM_USAGE_DEBUG
		XSEH::DecreaseAllocCount( "StructTitle" );
#endif
	}

	int					GetSID()					{ return m_nSID; }
	int					GetCode()					{ return m_nCode; }
	int					GetStatus()					{ return m_nStatus; }
	void				SetSID( int nSID )			{ m_nSID = nSID; }
	void				SetCode( int nCode )		{ m_nCode = nCode; }
	void				SetStatus( int nStatus )	{ m_nStatus = nStatus; }

	inline bool			GetDBInsertFlag()									{ return m_bDBInsertFlag; }
	inline void			SetDBInsertFlag( const bool bDBInsertFlag = true )	{ m_bDBInsertFlag = bDBInsertFlag; }
	inline bool			GetDBUpdateFlag()									{ return m_bDBUpdateFlag; }
	inline void			SetDBUpdateFlag( const bool bDBUpdateFlag = true )	{ m_bDBUpdateFlag = bDBUpdateFlag; }

protected:
	TitleEventHandler *	m_pHandler;
	int					m_nSID;
	int					m_nCode;
	int					m_nStatus;

	bool				m_bDBInsertFlag;
	bool				m_bDBUpdateFlag;
};