#pragma once

#ifdef _MEM_USAGE_DEBUG
#include <dump/XExceptionHandler.h>
#endif


struct StructTitleCondition
{
	static StructTitleCondition *	AllocTitleCondition( const int nSID, const int nType, const __int64 nCount );
	void							FreeTitleCondition();

	StructTitleCondition( const int nSID, const int nType, const __int64 nCount )
		: m_nSID( nSID ), m_nType( nType ), m_nCount( nCount ), m_bDBInsertFlag( false ), m_bDBUpdateFlag( false )
	{
#ifdef _MEM_USAGE_DEBUG
		XSEH::IncreaseAllocCount( "StructTitleCondition" );
#endif
	}

	~StructTitleCondition()
	{
#ifdef _MEM_USAGE_DEBUG
		XSEH::DecreaseAllocCount( "StructTitleCondition" );
#endif
	}

	const int			GetSID() const			{ return m_nSID; }
	const int			GetType() const			{ return m_nType; }
	const __int64		GetCount() const		{ return m_nCount; }

	void				SetSID( const int nSID )				{ m_nSID = nSID; }
	void				SetType( const int nType )				{ m_nType = nType; }
	void				SetCount( const __int64 nCount )		{ m_nCount = nCount; }
	void				AddCount( const __int64 nAdd )			{ m_nCount += nAdd; }

	inline bool			GetDBInsertFlag()									{ return m_bDBInsertFlag; }
	inline void			SetDBInsertFlag( const bool bDBInsertFlag = true )	{ m_bDBInsertFlag = bDBInsertFlag; }
	inline bool			GetDBUpdateFlag()									{ return m_bDBUpdateFlag; }
	inline void			SetDBUpdateFlag( const bool bDBUpdateFlag = true )	{ m_bDBUpdateFlag = bDBUpdateFlag; }

protected:
	int					m_nSID;
	int					m_nType;
	__int64				m_nCount;

	bool				m_bDBInsertFlag;
	bool				m_bDBUpdateFlag;
};