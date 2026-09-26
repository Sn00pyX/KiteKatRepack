#pragma once

#include <mmo/ArObject.h>
#include <mmo/ArTime.h>
#include <toolkit/ILock.h>

#include "StructBase.h"
#include "FieldPropBase.h"
#include "GameContent.h"


struct StructFieldProp : public GameObject
{
	// onFieldPropDelete 처리 루틴과 관련하여 StructFieldProp::FieldPropDeleter를 반드시 참조할 것
	struct FieldPropDeleteHandler
	{
		virtual void onFieldPropDelete( struct StructFieldProp * pProp )	= 0;
	};

	virtual ~StructFieldProp();

	AR_HANDLE					GetHandle() const	{ return m_hHandle; }

	static StructFieldProp*		Create( FieldPropDeleteHandler * pDeleteHandler, const GameContent::FIELD_PROP_RESPAWN_INFO* pPropInfo );
	static StructFieldProp*		Create( FieldPropDeleteHandler * pDeleteHandler, const GameContent::FIELD_PROP_RESPAWN_INFO* pPropInfo, const AR_UNIT & x, const AR_UNIT & y, const unsigned char nLayer );
	static StructFieldProp*		CreateWithoutAddingToWorld( FieldPropDeleteHandler * pDeleteHandler, const GameContent::FIELD_PROP_RESPAWN_INFO* pPropInfo );
	static StructFieldProp*		CreateWithoutAddingToWorld( FieldPropDeleteHandler * pDeleteHandler, const GameContent::FIELD_PROP_RESPAWN_INFO* pPropInfo, const AR_UNIT & x, const AR_UNIT & y, const unsigned char nLayer );
	static const bool			PendFreeFieldProp( StructFieldProp * pProp );
	bool						ProcDelete();
	void						SetDeleteHandler( FieldPropDeleteHandler * pDeleteHandler )			{ m_pDeleteHandler = pDeleteHandler; }
	
	virtual bool				IsFieldProp() const			{ return true; }

	int							GetPropId() const			{ return m_pPropInfo->nPropId; }
	float						GetZOffset() const			{ return m_pPropInfo->fZOffset; }
	float						GetRotateX() const			{ return m_pPropInfo->fRotateX; }
	float						GetRotateY() const			{ return m_pPropInfo->fRotateY; }
	float						GetRotateZ() const			{ return m_pPropInfo->fRotateZ; }
	float						GetScaleX() const			{ return m_pPropInfo->fScaleX; }
	float						GetScaleY() const			{ return m_pPropInfo->fScaleY; }
	float						GetScaleZ() const			{ return m_pPropInfo->fScaleZ; }

	bool						IsHeightLocked() const		{ return m_pPropInfo->bLockHeight; }
	float						GetLockHeight() const		{ return m_pPropInfo->fLockHeight; }

	bool						IsExpireObject() const		{ return !!m_pFieldPropBase->nLifeTime; }
	AR_TIME						GetRegenTime() const		{ return m_nRegenTime; }

	const FieldPropBase *							GetFieldPropBase() const	{ return m_pFieldPropBase; }
	const GameContent::FIELD_PROP_RESPAWN_INFO *	GetRespawnInfo() const		{ return m_pPropInfo; }

	int							GetRemainCount() const		{ return m_nUseCount; }

	bool						IsCastable( struct StructPlayer * pPlayer ) const;
	bool						IsUsable( struct StructPlayer * pPlayer ) const;
	bool						Cast();
	bool						CancelCast();
	bool						UseProp(struct StructPlayer * pPlayer );
	AR_TIME						GetCastingDelay() const;

private:
	StructFieldProp( FieldPropDeleteHandler * pDeleteHandler, const GameContent::FIELD_PROP_RESPAWN_INFO * pPropInfo );

	AR_HANDLE										m_hHandle;

	AR_TIME											m_nRegenTime;

	FieldPropDeleteHandler *						m_pDeleteHandler;
	const FieldPropBase *							m_pFieldPropBase;
	const GameContent::FIELD_PROP_RESPAWN_INFO *	m_pPropInfo;

	volatile LONG									m_nUseCount;

	bool											m_bIsCasting;

	// StructFieldProp은 ArcadiaServer::DeleteObject 함수를 사용해서 삭제하면 안되고 FieldPropDeleter::Push(SturctFieldProp::Delete가 호출함)를 사용해야 함
	// FieldPropDeleter는 onFieldPropDelete 함수를 아무 락도 걸지 않고 호출해 줌(StructFieldProp::onProcess이 없으므로 별도 수단이 필요하여 추가됨)
	struct FieldPropDeleter : public ArSchedulerObject
	{
		FieldPropDeleter()				{}
		virtual ~FieldPropDeleter();

		virtual void	onProcess( int nThreadIdx );

		const bool		Push( StructFieldProp * pProp );

	protected:
		XCriticalSection					m_csDelete;
		std::vector< StructFieldProp * >	m_vDeleteList;
	};

	static FieldPropDeleter							m_deleter;
};
