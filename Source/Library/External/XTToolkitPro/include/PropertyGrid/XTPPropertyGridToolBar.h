#ifndef XTPPropertyGridToolBar_h__
#define XTPPropertyGridToolBar_h__

//===========================================================================
// Summary:
//     CXTPPropertyGridToolBar is a CToolBar derived class.
//     It is an internal class used by Property Grid control
//===========================================================================
class _XTP_EXT_CLASS CXTPPropertyGridToolBar : public CToolBar
{
public:

	//-----------------------------------------------------------------------
	// Summary:
	//     Constructs a CXTPPropertyGridToolBar object
	//-----------------------------------------------------------------------
	CXTPPropertyGridToolBar();

	//-------------------------------------------------------------------------
	// Summary:
	//     Destroys a CXTPPropertyGridToolBar object, handles cleanup and deallocation.
	//-------------------------------------------------------------------------
	~CXTPPropertyGridToolBar();

protected:

	//{{AFX_CODEJOCK_PRIVATE
	DECLARE_MESSAGE_MAP()

	//{{AFX_VIRTUAL(CXTPPropertyGridToolBar)
	virtual void OnUpdateCmdUI(CFrameWnd* pTarget, BOOL bDisableIfNoHndler);
	virtual BOOL OnWndMsg(UINT message, WPARAM wParam, LPARAM lParam, LRESULT* pResult);
	//}}AFX_VIRTUAL

	//{{AFX_MSG(CXTPPropertyGridToolBar)
	afx_msg BOOL OnEraseBkgnd(CDC*);
	afx_msg void OnNcCalcSize(BOOL bCalcValidRects, NCCALCSIZE_PARAMS* lpncsp);
	afx_msg void OnPaint();
	afx_msg void OnCustomDraw(NMHDR* pNMHDR, LRESULT* pResult);
	//}}AFX_MSG
	//}}AFX_CODEJOCK_PRIVATE
};

#endif // XTPPropertyGridToolBar_h__
