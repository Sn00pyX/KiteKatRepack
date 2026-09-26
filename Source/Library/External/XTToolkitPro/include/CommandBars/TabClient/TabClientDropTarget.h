class CXTPTabClientWnd::CTabClientDropTarget : public COleDropTarget
{
public:
	CTabClientDropTarget();

	void OnDragLeave(CWnd *pWnd);

	virtual DROPEFFECT OnDragOver(CWnd* /*pWnd*/, COleDataObject* /*pDataObject*/, DWORD /*dwKeyState*/, CPoint point);

public:
	CXTPTabClientWnd* m_pTabClientWnd;

protected:
	DWORD m_dwDragHoverMode;
	DWORD m_dwDragLastTick;
	CPoint m_ptDragLastPoint;
};
