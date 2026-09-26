#pragma once

#include <mmo/ArSchedulerObject.h>
#include <toolkit/khash.h>
#include <toolkit/ILock.h>

#include "StructFieldProp.h"
#include "GameContent.h"

struct FieldPropManager : ArSchedulerObject, StructFieldProp::FieldPropDeleteHandler
{
public:
	~FieldPropManager();

	virtual void onFieldPropDelete( struct StructFieldProp * pProp );

	void onProcess( int nThreadIdx );

	static FieldPropManager&	GetInstance();

	bool					Init();
	bool					DeInit();

	void					RegisterFieldProp( const GameContent::FIELD_PROP_RESPAWN_INFO & prop );

	void					RegisterFieldPropSwitchingData( const FieldPropSwitchingData & switching );
	size_t					GetFieldPropSwitchingDataCount()	{ return m_vFieldPropSwitching.size(); }

	void					RegisterFieldPropBase( const FieldPropBase & base );
	const FieldPropBase &	GetFieldPropBase( int nPropId );

protected:
	FieldPropManager()
		: m_CS( "FieldPropManager::m_CS" )
		, m_ExpireCS( "FieldPropManager::m_ExpireCS" )
	{}

private:
	std::vector< FieldPropBase >			m_vFieldPropBase;
	std::vector< FieldPropSwitchingData >	m_vFieldPropSwitching;
	KHash< size_t, hashPr_mod_basic<int>  >	m_hsFieldPropId;

	std::vector< GameContent::FIELD_PROP_RESPAWN_INFO * >	m_vRespawnInfo;

	XCriticalSection										m_CS;
	std::vector< GameContent::REGEN_INFO >					m_vRespawnList;

	XCriticalSection										m_ExpireCS;
	std::vector< struct StructFieldProp * >					m_vExpireObject;
};