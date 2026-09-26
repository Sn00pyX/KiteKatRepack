class CXTPRibbonCustomizeTreeCtrl : public CXTPCoreTreeControl
{
public:
	CXTPRibbonCustomizeTreeCtrl();
protected:
	virtual void StartDragItem(CXTPCoreTreeItem* pItem);
	virtual BOOL OnDrop(COleDataObject* pDataObject, DROPEFFECT dropEffect, CPoint point);
	virtual DROPEFFECT OnDragOver(COleDataObject* pDataObject, DWORD dwKeyState, CPoint point);

	int GetItemLevel(CXTPCoreTreeItem* pItem) const;

public:
	void UpdateCommandBars();

public:
	BOOL m_bItemsTree;
	CXTPCommandBars* m_pCommandBars;
};
