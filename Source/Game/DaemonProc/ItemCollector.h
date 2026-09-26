#pragma once

#include <vector>

#include <toolkit/ILock.h>
#include <mmo/ArSchedulerObject.h>
#include <toolkit/XSTLUtil.h>


struct ItemCollector : ArSchedulerObject
{
public:
	static const int			IDX_RESERVED_TO_DELETE	= 0xFFFFFFFF;

public:
	bool Init();
	bool DeInit();

	void		RegisterItem( struct StructItem* pItem );
	const bool	UnregisterItem( struct StructItem* pItem );

	static ItemCollector & Instance();

	virtual void onProcess( int nThreadIdx );

protected:
	ItemCollector()
		: m_lock( "ItemCollector::m_lock" )
	{}

private:
	// 지역 락 -> m_lock 순서임
	XCriticalSection						m_lock;
	std::vector< struct StructItem* >		m_vItem;
	std::vector< struct StructItem* >		m_vTmp;
};
