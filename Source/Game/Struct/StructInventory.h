#pragma once

#include "ItemInstance.h"
#include "StructItem.h"
#include <vector>

struct StructInventory
{
	struct InventoryEventReceiver
	{
		virtual void onAdd( struct StructInventory* pInventory, struct StructItem* pItem, bool bSkipUpdateItemToDB = false )				{}
		virtual void onRemove( struct StructInventory* pInventory, struct StructItem* pItem, bool bSkipUpdateItemToDB = false )			{}
		virtual void onChangeCount( struct StructInventory* pInventory, struct StructItem* pItem, bool bSkipUpdateItemToDB = false )		{}
	};

	StructInventory( InventoryEventReceiver * pEventReceiver = NULL );
	virtual ~StructInventory();

	void					SetEventReceiver( InventoryEventReceiver * pEventReceiver )	{ m_pEventReceiver = pEventReceiver; }

	struct StructItem*		Join( struct StructItem* pItem, const __int64 & cnt, bool bSkipUpdateItemToDB = false );
	struct StructItem*		Push( struct StructItem* pItem, const __int64 & cnt, bool bSkipUpdateItemToDB = false );
	struct StructItem*		Pop( struct StructItem* pItem, const __int64 & cnt, bool bSkipUpdateItemToDB = false );

	struct StructItem*		Get( size_t idx )	{ return m_vList[idx]; }
	bool					Erase( struct StructItem* pItem, const __int64 & cnt, bool bSkipUpdateItemToDB = false );
	bool					Swap( int idx1, int idx2 );

	struct StructItem*		Find( ItemUID uid ) const;
	struct StructItem*		Find( ItemBase::ItemCode code ) const;
	struct StructItem*		Find( ItemBase::ItemCode code, unsigned int flag, bool bFlag ) const;
	void					Find( ItemBase::ItemCode, std::vector< StructItem * > & vList );
	struct StructItem*		FindJoinablePair( struct StructItem *pItem );
	const __int64			GetItemCount( ItemBase::ItemCode code ) const;

	size_t					GetCount() const	{ return m_vList.size(); }
	float					GetWeight() const	{ return m_fWeight + m_fWeightModifier; }
	float					GetWeightModifier() const		{ return m_fWeightModifier;	}
	void					SetWeightModifier( float fWeightModifier )		{ m_fWeightModifier = fWeightModifier; }
	void					AddWeightModifier( float fWeightModifier )		{ m_fWeightModifier += fWeightModifier; }

	bool					IsValid( struct StructItem* pItem ) const { return check( pItem ); }

	const std::vector< struct StructItem * > & GetExpireItemList() const				{ return m_vExpireItemList; }

	// 아이템 정렬 관련
	static const bool		_ItemArrangeGreater( const struct StructItem * lhs, const struct StructItem * rhs );
	void					Arrange();
	
	// 로그인, 창고 열기 등으로 아이템 목록을 한번에 넣을 때 사용
	void					LoadItemList( std::vector< StructItem::INDEX_FINDER > &vItemList, bool isStorage );

private:

	void					setCount( struct StructItem* pItem, const __int64 & new_cnt, bool bSkipUpdateItemToDB = false );
	void					push( struct StructItem* pItem, bool bSkipUpdateItemToDB = false );
	void					pop( struct StructItem* pItem, bool bSkipUpdateItemToDB = false );
	bool					check( struct StructItem* pItem ) const;

	InventoryEventReceiver*				m_pEventReceiver;
	float								m_fWeight;
	float								m_fWeightModifier;
	std::vector< struct StructItem * >	m_vList;
	std::vector< struct StructItem * >	m_vExpireItemList;
};